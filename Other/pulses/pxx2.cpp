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
#include "pulses/pxx2.h"
#include "io/frsky_firmware_update.h"

uint8_t Pxx2Pulses::addFlag0(uint8_t module)
{
  uint8_t flag0 = g_model.header.modelId[module] & 0x3F;
  if (g_model.moduleData[module].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[module].failsafeMode != FAILSAFE_RECEIVER) {
    if (moduleState[module].counter == 0) {
      flag0 |= PXX2_CHANNELS_FLAG0_FAILSAFE;
    }
  }
  if (moduleState[module].mode == MODULE_MODE_RANGECHECK) {
    flag0 |= PXX2_CHANNELS_FLAG0_RANGECHECK;
  }
  Pxx2Transport::addByte(flag0);
  return flag0;
}

void Pxx2Pulses::addFlag1(uint8_t module)
{
  uint8_t flag1 = g_model.moduleData[module].subType << 4;
  Pxx2Transport::addByte(flag1);
}

void Pxx2Pulses::addPulsesValues(uint16_t low, uint16_t high)
{
  Pxx2Transport::addByte(low); // Low byte of channel
  Pxx2Transport::addByte(((low >> 8) & 0x0F) | (high << 4));  // 4 bits each from 2 channels
  Pxx2Transport::addByte(high >> 4);  // High byte of channel
}

void Pxx2Pulses::addChannels(uint8_t module)
{
  uint16_t pulseValue = 0;
  uint16_t pulseValueLow = 0;

  uint8_t channel = g_model.moduleData[module].channelsStart;
  uint8_t count = sentModuleChannels(module);

  for (int8_t i = 0; i < count; i++, channel++) {
    int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
    pulseValue = limit(1, (value * 512 / 682) + 1024, 2046);
#if defined(DEBUG_LATENCY_RF_ONLY)
    if (latencyToggleSwitch)
      pulseValue = 1;
    else
      pulseValue = 2046;
#endif
    if (i & 1)
      addPulsesValues(pulseValueLow, pulseValue);
    else
      pulseValueLow = pulseValue;
  }
}

void Pxx2Pulses::addFailsafe(uint8_t module)
{
  uint16_t pulseValue = 0;
  uint16_t pulseValueLow = 0;

  uint8_t channel = g_model.moduleData[module].channelsStart;
  uint8_t count = sentModuleChannels(module);

  for (int8_t i = 0; i < count; i++, channel++) {
    if (g_model.moduleData[module].failsafeMode == FAILSAFE_HOLD) {
      pulseValue = 2047;
    }
    else if (g_model.moduleData[module].failsafeMode == FAILSAFE_NOPULSES) {
      pulseValue = 0;
    }
    else {
      int16_t failsafeValue = g_model.failsafeChannels[channel];
      if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
        pulseValue = 2047;
      }
      else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
        pulseValue = 0;
      }
      else {
        failsafeValue += 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(1, (failsafeValue * 512 / 682) + 1024, 2046);
      }
    }
    if (i & 1)
      addPulsesValues(pulseValueLow, pulseValue);
    else
      pulseValueLow = pulseValue;
  }
}

void Pxx2Pulses::setupChannelsFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_CHANNELS);

  // Flag0
  uint8_t flag0 = addFlag0(module);

  // Flag1
  addFlag1(module);

  // Failsafe / Channels
  if (flag0 & PXX2_CHANNELS_FLAG0_FAILSAFE)
    addFailsafe(module);
  else
    addChannels(module);
}

void Pxx2Pulses::setupTelemetryFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_TELEMETRY);
  Pxx2Transport::addByte(outputTelemetryBuffer.destination & 0x03);
  for (uint8_t i = 0; i < sizeof(SportTelemetryPacket); i++) {
    Pxx2Transport::addByte(outputTelemetryBuffer.data[i]);
  }
}

void Pxx2Pulses::setupHardwareInfoFrame(uint8_t module)
{
  ModuleInformation * destination = moduleState[module].moduleInformation;

  if (destination->timeout == 0) {
    if (destination->current <= destination->maximum) {
      addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_HW_INFO);
      Pxx2Transport::addByte(destination->current);
      destination->timeout = 60; /* 300ms */
      destination->current++;
    }
    else {
      moduleState[module].mode = MODULE_MODE_NORMAL;
      setupChannelsFrame(module);
    }
  }
  else {
    destination->timeout--;
    setupChannelsFrame(module);
  }
}

void Pxx2Pulses::setupRegisterFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_REGISTER);

  if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_RX_NAME_SELECTED) {
    Pxx2Transport::addByte(0x01);
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(zchar2char(reusableBuffer.moduleSetup.pxx2.registerRxName[i]));
    }
    for (uint8_t i=0; i<PXX2_LEN_REGISTRATION_ID; i++) {
      Pxx2Transport::addByte(zchar2char(g_model.modelRegistrationID[i]));
    }
    Pxx2Transport::addByte(reusableBuffer.moduleSetup.pxx2.registerLoopIndex);
  }
  else {
    Pxx2Transport::addByte(0);
  }
}

void Pxx2Pulses::setupModuleSettingsFrame(uint8_t module)
{
  ModuleSettings * destination = moduleState[module].moduleSettings;

  if (get_tmr10ms() > destination->retryTime) {
    addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_TX_SETTINGS);
    uint8_t flag0 = 0;
    if (destination->state == PXX2_SETTINGS_WRITE)
      flag0 |= PXX2_TX_SETTINGS_FLAG0_WRITE;
    Pxx2Transport::addByte(flag0);
    if (destination->state == PXX2_SETTINGS_WRITE) {
      uint8_t flag1 = 0;
      if (destination->externalAntenna)
        flag1 |= PXX2_TX_SETTINGS_FLAG1_EXTERNAL_ANTENNA;
      Pxx2Transport::addByte(flag1);
      Pxx2Transport::addByte(destination->txPower);
    }
    destination->retryTime = get_tmr10ms() + 200/*next try in 2s*/;
  }
  else {
    setupChannelsFrame(module);
  }
}

void Pxx2Pulses::setupReceiverSettingsFrame(uint8_t module)
{
  if (get_tmr10ms() > reusableBuffer.hardwareAndSettings.receiverSettings.timeout) {
    addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_RX_SETTINGS);
    uint8_t flag0 = reusableBuffer.hardwareAndSettings.receiverSettings.receiverId;
    if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_WRITE)
      flag0 |= PXX2_RX_SETTINGS_FLAG0_WRITE;
    Pxx2Transport::addByte(flag0);
    if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_WRITE) {
      uint8_t flag1 = 0;
      if (reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled)
        flag1 |= PXX2_RX_SETTINGS_FLAG1_TELEMETRY_DISABLED;
      if (reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate)
        flag1 |= PXX2_RX_SETTINGS_FLAG1_FASTPWM;
      Pxx2Transport::addByte(flag1);
      uint8_t outputsCount = min<uint8_t>(24, reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount);
      for (int i = 0; i < outputsCount; i++) {
        Pxx2Transport::addByte(min<uint8_t>(23, reusableBuffer.hardwareAndSettings.receiverSettings.outputsMapping[i]));
      }
    }
    reusableBuffer.hardwareAndSettings.receiverSettings.timeout = get_tmr10ms() + 200/*next try in 2s*/;
  }
  else {
    setupChannelsFrame(module);
  }
}

void Pxx2Pulses::setupAccstBindFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_BIND);
  Pxx2Transport::addByte(0x01); // DATA0
  for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
    Pxx2Transport::addByte(0x00);
  }
  Pxx2Transport::addByte((g_model.moduleData[module].pxx.receiver_channel_9_16 << 7) + (g_model.moduleData[module].pxx.receiver_telem_off << 6));
  Pxx2Transport::addByte(g_model.header.modelId[module]);
}

void Pxx2Pulses::setupAccessBindFrame(uint8_t module)
{
  BindInformation * destination = moduleState[module].bindInformation;

  if (destination->step == BIND_WAIT) {
    if (get_tmr10ms() > destination->timeout) {
      moduleState[module].mode = MODULE_MODE_NORMAL;
      destination->step = BIND_OK;
      POPUP_INFORMATION(STR_BIND_OK); // TODO rather use the new callback
    }
    return;
  }

  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_BIND);

  if (destination->step == BIND_INFO_REQUEST) {
    Pxx2Transport::addByte(0x02); // DATA0
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(destination->candidateReceiversNames[destination->selectedReceiverIndex][i]);
    }
  }
  else if (destination->step == BIND_START) {
    Pxx2Transport::addByte(0x01); // DATA0
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(destination->candidateReceiversNames[destination->selectedReceiverIndex][i]);
    }
    if (isModuleR9MAccess(module)) {
      Pxx2Transport::addByte((destination->lbtMode << 6) + (destination->flexMode << 4) + destination->rxUid); // RX_UID is the slot index (which is unique and never moved)
    }
    else {
      Pxx2Transport::addByte(0x00);
    }
    Pxx2Transport::addByte(g_model.header.modelId[module]);
  }
  else {
    Pxx2Transport::addByte(0x00); // DATA0
    for (uint8_t i=0; i<PXX2_LEN_REGISTRATION_ID; i++) {
      Pxx2Transport::addByte(zchar2char(g_model.modelRegistrationID[i]));
    }
  }
}

void Pxx2Pulses::setupResetFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_RESET);
  Pxx2Transport::addByte(reusableBuffer.moduleSetup.pxx2.resetReceiverIndex);
  Pxx2Transport::addByte(reusableBuffer.moduleSetup.pxx2.resetReceiverFlags);
  moduleState[module].mode = MODULE_MODE_NORMAL;
}

void Pxx2Pulses::setupSpectrumAnalyser(uint8_t module)
{
  if (reusableBuffer.spectrumAnalyser.dirty) {
    reusableBuffer.spectrumAnalyser.dirty = false;
    addFrameType(PXX2_TYPE_C_POWER_METER, PXX2_TYPE_ID_SPECTRUM);
    Pxx2Transport::addByte(0x00);
    Pxx2Transport::addWord(reusableBuffer.spectrumAnalyser.freq);
    Pxx2Transport::addWord(reusableBuffer.spectrumAnalyser.span);
    Pxx2Transport::addWord(reusableBuffer.spectrumAnalyser.step);
  }
}

void Pxx2Pulses::setupPowerMeter(uint8_t module)
{
  if (reusableBuffer.powerMeter.dirty) {
    reusableBuffer.powerMeter.dirty = false;
    addFrameType(PXX2_TYPE_C_POWER_METER, PXX2_TYPE_ID_POWER_METER);
    Pxx2Transport::addByte(0x00);
    Pxx2Transport::addWord(reusableBuffer.powerMeter.freq);
  }
}

void Pxx2Pulses::setupShareMode(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_SHARE);
  Pxx2Transport::addByte(reusableBuffer.moduleSetup.pxx2.shareReceiverIndex);
}

void Pxx2Pulses::sendOtaUpdate(uint8_t module, const char * rxName, uint32_t address, const char * data)
{
  initFrame();

  addFrameType(PXX2_TYPE_C_OTA, PXX2_TYPE_ID_OTA);

  if (rxName) {
    Pxx2Transport::addByte(0x00);
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(rxName[i]);
    }
  }
  else if (data) {
    Pxx2Transport::addByte(0x01);
    Pxx2Transport::addWord(address);
    for (uint8_t i=0; i<32; i++) {
      Pxx2Transport::addByte(data[i]);
    }
  }
  else {
    Pxx2Transport::addByte(0x02);
  }

  endFrame();

  if (module == EXTERNAL_MODULE)
    extmoduleSendNextFrame();
}

void Pxx2Pulses::setupFrame(uint8_t module)
{
  if (moduleState[module].mode == MODULE_MODE_OTA_UPDATE)
    return;

  initFrame();

  switch (moduleState[module].mode) {
    case MODULE_MODE_GET_HARDWARE_INFO:
      setupHardwareInfoFrame(module);
      break;
    case MODULE_MODE_MODULE_SETTINGS:
      setupModuleSettingsFrame(module);
      break;
    case MODULE_MODE_RECEIVER_SETTINGS:
      setupReceiverSettingsFrame(module);
      break;
    case MODULE_MODE_REGISTER:
      setupRegisterFrame(module);
      break;
    case MODULE_MODE_BIND:
      if (g_model.moduleData[module].type == MODULE_TYPE_ISRM_PXX2 && g_model.moduleData[module].subType != MODULE_SUBTYPE_ISRM_PXX2_ACCESS)
        setupAccstBindFrame(module);
      else
        setupAccessBindFrame(module);
      break;
    case MODULE_MODE_RESET:
      setupResetFrame(module);
      break;
    case MODULE_MODE_SPECTRUM_ANALYSER:
      setupSpectrumAnalyser(module);
      break;
    case MODULE_MODE_POWER_METER:
      setupPowerMeter(module);
      break;
    case MODULE_MODE_SHARE:
      setupShareMode(module);
      break;
    default:
      if (outputTelemetryBuffer.isModuleDestination(module)) {
        setupTelemetryFrame(module);
        outputTelemetryBuffer.reset();
      }
      else {
        setupChannelsFrame(module);
      }
      break;
  }

  if (moduleState[module].counter-- == 0) {
    moduleState[module].counter = 2500;
  }

  endFrame();
}

bool Pxx2OtaUpdate::waitStep(uint8_t step, uint8_t timeout)
{
  OtaUpdateInformation * destination = moduleState[module].otaUpdateInformation;
  uint8_t elapsed = 0;

  watchdogSuspend(100);

  while (step != destination->step) {
    if (elapsed++ > timeout) {
      return false;
    }
    RTOS_WAIT_MS(1);
    telemetryWakeup();
  }

  return true;
}

const char * Pxx2OtaUpdate::nextStep(uint8_t step, const char * rxName, uint32_t address, const uint8_t * buffer)
{
  OtaUpdateInformation * destination = moduleState[module].otaUpdateInformation;

  destination->step = step;
  destination->address = address;

  for (uint8_t retry = 0;; retry++) {
    extmodulePulsesData.pxx2.sendOtaUpdate(module, rxName, address, (const char *) buffer);
    if (waitStep(step + 1, 20)) {
      return nullptr;
    }
    else if (retry == 100) {
      return "Transfer failed";
    }
  }
}

const char * Pxx2OtaUpdate::doFlashFirmware(const char * filename)
{
  FIL file;
  uint8_t buffer[32];
  UINT count;
  const char * result;

  result = nextStep(OTA_UPDATE_START, rxName, 0, nullptr);
  if (result) {
    return result;
  }

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Open file failed";
  }

  uint32_t size;
  const char * ext = getFileExtension(filename);
  if (ext && !strcasecmp(ext, UPDATE_FIRMWARE_EXT)) {
    FrSkyFirmwareInformation * information = (FrSkyFirmwareInformation *) buffer;
    if (f_read(&file, buffer, sizeof(FrSkyFirmwareInformation), &count) != FR_OK || count != sizeof(FrSkyFirmwareInformation)) {
      f_close(&file);
      return "Format error";
    }
    size = information->size;
  }
  else {
    size = f_size(&file);
  }

  uint32_t done = 0;
  while (1) {
    drawProgressScreen(getBasename(filename), STR_OTA_UPDATE, done, size);
    if (f_read(&file, buffer, sizeof(buffer), &count) != FR_OK) {
      f_close(&file);
      return "Read file failed";
    }

    result = nextStep(OTA_UPDATE_TRANSFER, nullptr, done, buffer);
    if (result) {
      return result;
    }

    if (count < sizeof(buffer)) {
      f_close(&file);
      break;
    }

    done += count;
  }

  return nextStep(OTA_UPDATE_EOF, nullptr, done, nullptr);
}

void Pxx2OtaUpdate::flashFirmware(const char * filename)
{
  pausePulses();

  watchdogSuspend(100);
  RTOS_WAIT_MS(100);

  moduleState[module].mode = MODULE_MODE_OTA_UPDATE;
  const char * result = doFlashFirmware(filename);
  moduleState[module].mode = MODULE_MODE_NORMAL;

  AUDIO_PLAY(AU_SPECIAL_SOUND_BEEP1 );
  BACKLIGHT_ENABLE();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }
  else {
    POPUP_INFORMATION(STR_FIRMWARE_UPDATE_SUCCESS);
  }

  watchdogSuspend(100);
  RTOS_WAIT_MS(100);

  resumePulses();
}
