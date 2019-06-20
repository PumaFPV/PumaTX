/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "io/frsky_firmware_update.h"

#if defined(PCBHORUS) || defined(PCBX7) || defined(PCBXLITE)
#define BLUETOOTH_COMMAND_NAME         "AT+NAME"
#define BLUETOOTH_ANSWER_NAME          "OK+"
#define BLUETOOTH_COMMAND_BAUD_115200  "AT+BAUD115200"
#else
#define BLUETOOTH_COMMAND_NAME         "TTM:REN-"
#define BLUETOOTH_ANSWER_NAME          "TTM:REN"
#define BLUETOOTH_COMMAND_BAUD_115200  "TTM:BPS-115200"
#endif

#if defined(_MSC_VER)
  #define SWAP32(val)      (_byteswap_ulong(val))
#elif defined(__GNUC__ )
  #define SWAP32(val)      (__builtin_bswap32(val))
#endif

extern Fifo<uint8_t, BT_TX_FIFO_SIZE> btTxFifo;
extern Fifo<uint8_t, BT_RX_FIFO_SIZE> btRxFifo;

Bluetooth bluetooth;

void Bluetooth::write(const uint8_t * data, uint8_t length)
{
  TRACE_NOCRLF("BT>");
  for (int i=0; i<length; i++) {
    TRACE_NOCRLF(" %02X", data[i]);
    if (btTxFifo.isFull()) {
      if (!bluetoothIsWriting())
        bluetoothWriteWakeup();
      RTOS_WAIT_MS(1);
    }
    btTxFifo.push(data[i]);
  }
  TRACE_NOCRLF("\r\n");
  bluetoothWriteWakeup();
}

void Bluetooth::writeString(const char * str)
{
  TRACE("BT> %s", str);
  while (*str != 0) {
    btTxFifo.push(*str++);
  }
  btTxFifo.push('\r');
  btTxFifo.push('\n');
  bluetoothWriteWakeup();
}

char * Bluetooth::readline(bool error_reset)
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
#if defined(PCBX9E)
      // X9E BT module can get unresponsive
      TRACE("NO RESPONSE FROM BT MODULE");
#endif
      return nullptr;
    }

    TRACE_NOCRLF("%02X ", byte);

#if 0
    if (error_reset && byte == 'R' && bufferIndex == 4 && memcmp(buffer, "ERRO", 4)) {
#if defined(PCBX9E)  // X9E enter BT reset loop if following code is implemented
      TRACE("BT Error...");
#else
      TRACE("BT Reset...");
      bufferIndex = 0;
      bluetoothDisable();
      state = BLUETOOTH_STATE_OFF;
      wakeupTime = get_tmr10ms() + 100; /* 1s */
#endif
      return NULL;
    }
    else
#endif

    if (byte == '\n') {
      if (bufferIndex > 2 && buffer[bufferIndex-1] == '\r') {
        buffer[bufferIndex-1] = '\0';
        bufferIndex = 0;
        TRACE("BT< %s", buffer);
        if (error_reset && !strcmp((char *)buffer, "ERROR")) {
#if defined(PCBX9E) // X9E enter BT reset loop if following code is implemented
          TRACE("BT Error...");
#else
          TRACE("BT Reset...");
          bluetoothDisable();
          state = BLUETOOTH_STATE_OFF;
          wakeupTime = get_tmr10ms() + 100; /* 1s */
#endif
          return nullptr;
        }
        else {
          if (!memcmp(buffer, "Central:", 8))
            strcpy(localAddr, (char *) buffer + 8);
          else if (!memcmp(buffer, "Peripheral:", 11))
            strcpy(localAddr, (char *) buffer + 11);
          return (char *) buffer;
        }
      }
      else {
        bufferIndex = 0;
      }
    }
    else {
      buffer[bufferIndex++] = byte;
      bufferIndex &= (BLUETOOTH_LINE_LENGTH-1);
    }
  }
}

void Bluetooth::processTrainerFrame(const uint8_t * buffer)
{
  TRACE("");

  for (uint8_t channel=0, i=1; channel<8; channel+=2, i+=3) {
    // +-500 != 512, but close enough.
    ppmInput[channel] = buffer[i] + ((buffer[i+1] & 0xf0) << 4) - 1500;
    ppmInput[channel+1] = ((buffer[i+1] & 0x0f) << 4) + ((buffer[i+2] & 0xf0) >> 4) + ((buffer[i+2] & 0x0f) << 8) - 1500;
  }

  ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
}

void Bluetooth::appendTrainerByte(uint8_t data)
{
  if (bufferIndex < BLUETOOTH_LINE_LENGTH) {
    buffer[bufferIndex++] = data;
    // we check for "DisConnected", but the first byte could be altered (if received in state STATE_DATA_XOR)
    if (data == '\n') {
      if (!strncmp((char *)&buffer[bufferIndex-13], "isConnected", 11)) {
        TRACE("BT< DisConnected");
        state = BLUETOOTH_STATE_DISCONNECTED;
        bufferIndex = 0;
        wakeupTime += 200; // 1s
      }
    }
  }
}

void Bluetooth::processTrainerByte(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
    case STATE_DATA_START:
      if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bufferIndex = 0;
      }
      else {
        appendTrainerByte(data);
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bufferIndex = 0;
      }
      else {
        appendTrainerByte(data);
      }
      break;

    case STATE_DATA_XOR:
      appendTrainerByte(data ^ STUFF_MASK);
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        bufferIndex = 0;
        dataState = STATE_DATA_START;
      }
      else {
        appendTrainerByte(data);
      }
      break;
  }

  if (bufferIndex >= BLUETOOTH_PACKET_SIZE) {
    uint8_t crc = 0x00;
    for (int i=0; i<13; i++) {
      crc ^= buffer[i];
    }
    if (crc == buffer[13]) {
      if (buffer[0] == 0x80) {
        processTrainerFrame(buffer);
      }
    }
    dataState = STATE_DATA_IDLE;
  }
}

void Bluetooth::pushByte(uint8_t byte)
{
  crc ^= byte;
  if (byte == START_STOP || byte == BYTESTUFF) {
    buffer[bufferIndex++] = 0x7d;
    byte ^= STUFF_MASK;
  }
  buffer[bufferIndex++] = byte;
}

void Bluetooth::sendTrainer()
{
  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;

  int firstCh = g_model.trainerData.channelsStart;
  int lastCh = firstCh + 8;

  uint8_t * cur = buffer;
  bufferIndex = 0;
  crc = 0x00;

  buffer[bufferIndex++] = START_STOP; // start byte
  pushByte(0x80); // trainer frame type?
  for (int channel=0; channel<lastCh; channel+=2, cur+=3) {
    uint16_t channelValue1 = PPM_CH_CENTER(channel) + limit((int16_t)-PPM_range, channelOutputs[channel], (int16_t)PPM_range) / 2;
    uint16_t channelValue2 = PPM_CH_CENTER(channel+1) + limit((int16_t)-PPM_range, channelOutputs[channel+1], (int16_t)PPM_range) / 2;
    pushByte(channelValue1 & 0x00ff);
    pushByte(((channelValue1 & 0x0f00) >> 4) + ((channelValue2 & 0x00f0) >> 4));
    pushByte(((channelValue2 & 0x000f) << 4) + ((channelValue2 & 0x0f00) >> 8));
  }
  buffer[bufferIndex++] = crc;
  buffer[bufferIndex++] = START_STOP; // end byte

  write(buffer, bufferIndex);
  bufferIndex = 0;
}

void Bluetooth::forwardTelemetry(const uint8_t * packet)
{
  crc = 0x00;

  buffer[bufferIndex++] = START_STOP; // start byte
  for (uint8_t i=0; i<sizeof(SportTelemetryPacket); i++) {
    pushByte(packet[i]);
  }
  buffer[bufferIndex++] = crc;
  buffer[bufferIndex++] = START_STOP; // end byte

  if (bufferIndex >= 2*FRSKY_SPORT_PACKET_SIZE) {
    write(buffer, bufferIndex);
    bufferIndex = 0;
  }
}

void Bluetooth::receiveTrainer()
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
      return;
    }

    TRACE_NOCRLF("%02X ", byte);

    processTrainerByte(byte);
  }
}

#if defined(PCBX9E)
void Bluetooth::wakeup(void)
{
#if !defined(SIMU)
  if (!g_eeGeneral.bluetoothMode) {
    if (state != BLUETOOTH_INIT) {
      bluetoothDisable();
      state = BLUETOOTH_INIT;
    }
  }
  else {
    static tmr10ms_t waitEnd = 0;
    if (state != BLUETOOTH_STATE_IDLE) {
      if (state == BLUETOOTH_INIT) {
        bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE, true);
        char command[32];
        char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
        uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
        if (len > 0) {
          for (int i = 0; i < len; i++) {
            *cur++ = char2lower(zchar2char(g_eeGeneral.bluetoothName[i]));
          }
          *cur = '\0';
        }
        else {
          cur = strAppend(cur, FLAVOUR);
        }
        writeString(command);
        state = BLUETOOTH_WAIT_TTM;
        waitEnd = get_tmr10ms() + 25; // 250ms
      }
      else if (state == BLUETOOTH_WAIT_TTM) {
        if (get_tmr10ms() > waitEnd) {
          char * line = readline();
          if (strncmp(line, "OK+", 3)) {
            state = BLUETOOTH_STATE_IDLE;
          }
          else {
            bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE, true);
            writeString("TTM:BPS-115200");
            state = BLUETOOTH_WAIT_BAUDRATE_CHANGE;
            waitEnd = get_tmr10ms() + 250; // 2.5s
          }
        }
      }
      else if (state == BLUETOOTH_WAIT_BAUDRATE_CHANGE) {
        if (get_tmr10ms() > waitEnd) {
          state = BLUETOOTH_INIT;
        }
      }
    }
    else if (IS_BLUETOOTH_TRAINER()){
      state = BLUETOOTH_STATE_CONNECTED;
      bluetoothWriteWakeup();
      sendTrainer();
    }
  }
#endif
}
#else // PCBX9E
void Bluetooth::wakeup()
{
  if (state != BLUETOOTH_STATE_OFF) {
    bluetoothWriteWakeup();
    if (bluetoothIsWriting()) {
      return;
    }
  }

  tmr10ms_t now = get_tmr10ms();

  if (now < wakeupTime)
    return;

  wakeupTime = now + 5; /* 50ms default */

  if (state == BLUETOOTH_STATE_FLASH_FIRMWARE) {
    return;
  }

  if (g_eeGeneral.bluetoothMode == BLUETOOTH_OFF || (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && !IS_BLUETOOTH_TRAINER())) {
    if (state != BLUETOOTH_STATE_OFF) {
      bluetoothDisable();
      state = BLUETOOTH_STATE_OFF;
    }
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_OFF) {
    bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE, true);
    state = BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT;
  }

  if (state == BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT) {
    writeString("AT+BAUD4");
    state = BLUETOOTH_STATE_BAUDRATE_SENT;
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_BAUDRATE_SENT) {
    bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE, true);
    state = BLUETOOTH_STATE_BAUDRATE_INIT;
    readline(false);
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_CONNECTED) {
    if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH) {
      receiveTrainer();
    }
    else {
      if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH) {
        sendTrainer();
        wakeupTime = now + 2; /* 20ms */
      }
      readline(); // to deal with "ERROR"
    }
  }
  else {
    char * line = readline();
    if (state == BLUETOOTH_STATE_BAUDRATE_INIT) {
      char command[32];
      char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
      uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
      if (len > 0) {
        for (int i = 0; i < len; i++) {
          *cur++ = char2lower(zchar2char(g_eeGeneral.bluetoothName[i]));
        }
        *cur = '\0';
      }
      else {
        cur = strAppend(cur, FLAVOUR);
      }
      writeString(command);
      state = BLUETOOTH_STATE_NAME_SENT;
    }
    else if (state == BLUETOOTH_STATE_NAME_SENT && (!strncmp(line, "OK+", 3) || !strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      writeString("AT+TXPW0");
      state = BLUETOOTH_STATE_POWER_SENT;
    }
    else if (state == BLUETOOTH_STATE_POWER_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH)
        writeString("AT+ROLE1");
      else
        writeString("AT+ROLE0");
      state = BLUETOOTH_STATE_ROLE_SENT;
    }
    else if (state == BLUETOOTH_STATE_ROLE_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      state = BLUETOOTH_STATE_IDLE;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_REQUESTED) {
      writeString("AT+DISC?");
      state = BLUETOOTH_STATE_DISCOVER_SENT;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_SENT && !strcmp(line, "OK+DISCS")) {
      state = BLUETOOTH_STATE_DISCOVER_START;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_START && !strncmp(line, "OK+DISC:", 8)) {
      if (strlen(line) < 8 + LEN_BLUETOOTH_ADDR && reusableBuffer.moduleSetup.bt.devicesCount < MAX_BLUETOOTH_DISTANT_ADDR) {
        strncpy(reusableBuffer.moduleSetup.bt.devices[reusableBuffer.moduleSetup.bt.devicesCount], &line[8], LEN_BLUETOOTH_ADDR);
        ++reusableBuffer.moduleSetup.bt.devicesCount;
      }
    }
    /* else if (state == BLUETOOTH_STATE_DISCOVER_START && !strcmp(line, "OK+DISCE")) {
      state = BLUETOOTH_STATE_DISCOVER_END;
    } */
    else if (state == BLUETOOTH_STATE_CLEAR_REQUESTED) {
      char command[] = "AT+CLEAR";
      writeString(command);
      state = BLUETOOTH_STATE_IDLE;
    }
    else if (state == BLUETOOTH_STATE_BIND_REQUESTED) {
      char command[32];
      strAppend(strAppend(command, "AT+CON"), distantAddr);
      writeString(command);
      state = BLUETOOTH_STATE_CONNECT_SENT;
    }
    else if ((state == BLUETOOTH_STATE_IDLE || state == BLUETOOTH_STATE_DISCONNECTED || state == BLUETOOTH_STATE_CONNECT_SENT) && !strncmp(line, "Connected:", 10)) {
      strcpy(distantAddr, &line[10]); // TODO quick & dirty
      state = BLUETOOTH_STATE_CONNECTED;
      if (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH) {
        wakeupTime += 500; // it seems a 5s delay is needed before sending the 1st frame
      }
    }
    else if (state == BLUETOOTH_STATE_DISCONNECTED && !line) {
      char command[32];
      strAppend(strAppend(command, "AT+CON"), distantAddr);
      writeString(command);
      wakeupTime = now + 200; /* 2s */
    }
  }
}
#endif

uint8_t Bluetooth::bootloaderChecksum(uint8_t command, const uint8_t * data, uint8_t size)
{
  uint8_t sum = command;
  for (uint8_t i = 0; i < size; i++) {
    sum += data[i];
  }
  return sum;
}

uint8_t Bluetooth::read(uint8_t * data, uint8_t size, uint8_t timeout)
{
  watchdogSuspend(timeout / 10);

  uint8_t len = 0;
  while (len < size) {
    uint8_t elapsed = 0;
    uint8_t byte;
    while (!btRxFifo.pop(byte)) {
      if (elapsed++ >= timeout) {
        return len;
      }
      RTOS_WAIT_MS(1);
    }
    data[len++] = byte;
  }
  return len;
}

#define BLUETOOTH_ACK   0xCC
#define BLUETOOTH_NACK  0x33

const char * Bluetooth::bootloaderWaitCommandResponse(uint8_t timeout)
{
  uint8_t response[2];
  if (read(response, sizeof(response), timeout) != sizeof(response)) {
    return "Bluetooth timeout";
  }

  if (response[0] != 0x00) {
    return "Bluetooth error";
  }

  if (response[1] == BLUETOOTH_ACK || response[1] == BLUETOOTH_NACK) {
    return nullptr;
  }

  return "Bluetooth error";
}

const char * Bluetooth::bootloaderWaitResponseData(uint8_t *data, uint8_t size)
{
  uint8_t header[2];
  if (read(header, 2) != 2) {
    return "Bluetooth timeout";
  }

  uint8_t len = header[0] - 2;
  uint8_t checksum = header[1];

  if (len > size) {
    return "Bluetooth error";
  }

  if (read(data, len) != len) {
    return "Bluetooth timeout";
  }

  if (bootloaderChecksum(0, data, len) != checksum) {
    return "Bluetooth CRC error";
  }

  return nullptr;
}

const char * Bluetooth::bootloaderSetAutoBaud()
{
  uint8_t packet[2] = {
    0x55,
    0x55
  };
  write(packet, sizeof(packet));
  return bootloaderWaitCommandResponse();
}

void Bluetooth::bootloaderSendCommand(uint8_t command, const void *data, uint8_t size)
{
  uint8_t packet[3] = {
    uint8_t(3 + size),
    bootloaderChecksum(command, (uint8_t *) data, size),
    command
  };
  write(packet, sizeof(packet));
  if (size > 0) {
    write((uint8_t *)data, size);
  }
}

void Bluetooth::bootloaderSendCommandResponse(uint8_t response)
{
  uint8_t packet[2] = {
    0x00,
    response
  };
  write(packet, sizeof(packet));
}

enum {
  CMD_DOWNLOAD = 0x21,
  CMD_GET_STATUS = 0x23,
  CMD_SEND_DATA = 0x24,
  CMD_SECTOR_ERASE = 0x26,
  CMD_GET_CHIP_ID = 0x28,
};

enum {
  CMD_RET_SUCCESS = 0x40,
};

constexpr uint32_t CC26XX_FLASH_SIZE = 0x00020000;
constexpr uint32_t CC26XX_BOOTLOADER_SIZE = 0x00001000;
constexpr uint32_t CC26XX_DATA_SIZE = 0x00001000;
constexpr uint32_t CC26XX_FIRMWARE_BASE = CC26XX_BOOTLOADER_SIZE;
constexpr uint32_t CC26XX_FIRMWARE_SIZE = CC26XX_FLASH_SIZE - CC26XX_DATA_SIZE - CC26XX_BOOTLOADER_SIZE;

constexpr uint32_t CC26XX_PAGE_ERASE_SIZE = 0x1000;
constexpr uint32_t CC26XX_MAX_BYTES_PER_TRANSFER = 252;

const char * Bluetooth::bootloaderSendData(const uint8_t * data, uint8_t size)
{
  bootloaderSendCommand(CMD_SEND_DATA, data, size);
  return bootloaderWaitCommandResponse();
}

const char * Bluetooth::bootloaderReadStatus(uint8_t &status)
{
  bootloaderSendCommand(CMD_GET_STATUS);
  const char * result = bootloaderWaitCommandResponse();
  if (result)
    return result;
  result = bootloaderWaitResponseData(&status, 1);
  bootloaderSendCommandResponse(result == nullptr ? BLUETOOTH_ACK : BLUETOOTH_NACK);
  return result;
}

const char * Bluetooth::bootloaderCheckStatus()
{
  uint8_t status;
  const char * result = bootloaderReadStatus(status);
  if (result)
    return result;
  if (status != CMD_RET_SUCCESS)
    return "Wrong status";
  return nullptr;
}

const char * Bluetooth::bootloaderEraseFlash(uint32_t start, uint32_t size)
{
  uint32_t address = start;
  uint32_t end = start + size;
  while (address < end) {
    uint32_t addressBigEndian = SWAP32(address);
    bootloaderSendCommand(CMD_SECTOR_ERASE, &addressBigEndian, sizeof(addressBigEndian));
    const char * result = bootloaderWaitCommandResponse();
    if (result)
      return result;
    result = bootloaderCheckStatus();
    if (result)
      return result;
    address += CC26XX_PAGE_ERASE_SIZE;
  }
  return nullptr;
}

const char * Bluetooth::bootloaderStartWriteFlash(uint32_t start, uint32_t size)
{
  uint32_t cmdArgs[2] = {
    SWAP32(start),
    SWAP32(size),
  };
  bootloaderSendCommand(CMD_DOWNLOAD, cmdArgs, sizeof(cmdArgs));
  const char * result = bootloaderWaitCommandResponse();
  if (result)
    return result;
  result = bootloaderCheckStatus();
  if (result)
    return result;
  return result;
}

const char * Bluetooth::bootloaderWriteFlash(const uint8_t * data, uint32_t size)
{
  while (size > 0) {
    uint32_t len = min<uint32_t>(size, CC26XX_MAX_BYTES_PER_TRANSFER);
    const char * result = bootloaderSendData(data, len);
    if (result)
      return result;
    result = bootloaderCheckStatus();
    if (result)
      return result;
    data += len;
    size -= len;
  }
  return nullptr;
}

const char * Bluetooth::doFlashFirmware(const char * filename)
{
  const char * result;
  FIL file;
  uint8_t buffer[CC26XX_MAX_BYTES_PER_TRANSFER * 4];
  UINT count;

  // Dummy command
  bootloaderSendCommand(0);
  result = bootloaderWaitCommandResponse(0);
  if (result)
    result = bootloaderSetAutoBaud();
  if (result)
    return result;

  // Get chip ID
  bootloaderSendCommand(CMD_GET_CHIP_ID);
  result = bootloaderWaitCommandResponse();
  if (result)
    return result;
  uint8_t id[4];
  result = bootloaderWaitResponseData(id, 4);
  bootloaderSendCommandResponse(result == nullptr ? BLUETOOTH_ACK : BLUETOOTH_NACK);

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Error opening file";
  }

  FrSkyFirmwareInformation * information = (FrSkyFirmwareInformation *)buffer;
  if (f_read(&file, buffer, sizeof(FrSkyFirmwareInformation), &count) != FR_OK || count != sizeof(FrSkyFirmwareInformation)) {
    f_close(&file);
    return "Format error";
  }

  drawProgressScreen(getBasename(filename), STR_FLASH_ERASE, 0, 0);

  result = bootloaderEraseFlash(CC26XX_FIRMWARE_BASE, information->size);
  if (result) {
    f_close(&file);
    return result;
  }

  uint32_t size = information->size;
  drawProgressScreen(getBasename(filename), STR_FLASH_WRITE, 0, size);

  result = bootloaderStartWriteFlash(CC26XX_FIRMWARE_BASE, size);
  if (result)
    return result;

  uint32_t done = 0;
  while (1) {
    drawProgressScreen(getBasename(filename), STR_FLASH_WRITE, done, size);
    if (f_read(&file, buffer, min<uint32_t>(sizeof(buffer), size - done), &count) != FR_OK) {
      f_close(&file);
      return "Error reading file";
    }
    result = bootloaderWriteFlash(buffer, count);
    if (result)
      return result;
    done += count;
    if (done >= size) {
      f_close(&file);
      return nullptr;
    }
  }
}

void Bluetooth::flashFirmware(const char * filename)
{
  drawProgressScreen(getBasename(filename), STR_MODULE_RESET, 0, 0);

  state = BLUETOOTH_STATE_FLASH_FIRMWARE;

  pausePulses();

  bluetoothInit(BLUETOOTH_BOOTLOADER_BAUDRATE, true); // normal mode
  watchdogSuspend(1000);
  RTOS_WAIT_MS(1000);

  bluetoothInit(BLUETOOTH_BOOTLOADER_BAUDRATE, false); // bootloader mode
  watchdogSuspend(1000);
  RTOS_WAIT_MS(1000);

  const char * result = doFlashFirmware(filename);

  AUDIO_PLAY(AU_SPECIAL_SOUND_BEEP1 );
  BACKLIGHT_ENABLE();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }
  else {
    POPUP_INFORMATION(STR_FIRMWARE_UPDATE_SUCCESS);
  }

  drawProgressScreen(getBasename(filename), STR_MODULE_RESET, 0, 0);

  /* wait 1s off */
  watchdogSuspend(1000);
  RTOS_WAIT_MS(1000);

  state = BLUETOOTH_STATE_OFF;
  resumePulses();
}
