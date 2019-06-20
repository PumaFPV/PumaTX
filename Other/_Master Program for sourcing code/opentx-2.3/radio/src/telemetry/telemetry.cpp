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

#include <opentx.h>

uint8_t telemetryStreaming = 0;
uint8_t R9ModuleStreaming = 0;
uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t telemetryRxBufferCount = 0;

#if defined(WS_HOW_HIGH)
uint8_t wshhStreaming = 0;
#endif

uint8_t telemetryState = TELEMETRY_INIT;

TelemetryData telemetryData;

uint8_t telemetryProtocol = 255;

#if defined(PCBSKY9X) && defined(REVX)
uint8_t serialInversion = 0;
#endif

void processTelemetryData(uint8_t data)
{
#if defined(CROSSFIRE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
    processCrossfireTelemetryData(data);
    return;
  }
#endif

#if defined(MULTIMODULE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_SPEKTRUM) {
    processSpektrumTelemetryData(data);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    processFlySkyTelemetryData(data);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_MULTIMODULE) {
    processMultiTelemetryData(data);
    return;
  }
#endif

  processFrskyTelemetryData(data);
}

inline bool isBadAntennaDetected()
{
  if (!isRasValueValid())
    return false;

  if (telemetryData.swrInternal.isFresh() && telemetryData.swrInternal.value > FRSKY_BAD_ANTENNA_THRESHOLD)
    return true;

  if (telemetryData.swrExternal.isFresh() && telemetryData.swrExternal.value > FRSKY_BAD_ANTENNA_THRESHOLD)
    return true;

  return false;
}

void telemetryWakeup()
{
  uint8_t requiredTelemetryProtocol = modelTelemetryProtocol();

#if defined(REVX)
  uint8_t requiredSerialInversion = g_model.moduleData[EXTERNAL_MODULE].invertedSerial;
  if (telemetryProtocol != requiredTelemetryProtocol || serialInversion != requiredSerialInversion) {
    serialInversion = requiredSerialInversion;
    telemetryInit(requiredTelemetryProtocol);
  }
#else
  if (telemetryProtocol != requiredTelemetryProtocol) {
    telemetryInit(requiredTelemetryProtocol);
  }
#endif

#if defined(INTMODULE_USART) || defined(EXTMODULE_USART)
  uint8_t frame[PXX2_FRAME_MAXLENGTH];

  #if defined(INTMODULE_USART)
    while (intmoduleFifo.getFrame(frame)) {
      processPXX2Frame(INTERNAL_MODULE, frame);
    }
  #endif

  #if defined(EXTMODULE_USART)
    while (extmoduleFifo.getFrame(frame)) {
      processPXX2Frame(EXTERNAL_MODULE, frame);
    }
  #endif
#endif

#if defined(STM32)
  uint8_t data;
  if (telemetryGetByte(&data)) {
    LOG_TELEMETRY_WRITE_START();
    do {
      processTelemetryData(data);
      LOG_TELEMETRY_WRITE_BYTE(data);
    } while (telemetryGetByte(&data));
  }
#elif defined(PCBSKY9X)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    uint8_t data;
    while (telemetrySecondPortReceive(data)) {
      processTelemetryData(data);
    }
  }
  else {
    // Receive serial data here
    rxPdcUsart(processTelemetryData);
  }
#endif

  for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
    const TelemetrySensor & sensor = g_model.telemetrySensors[i];
    if (sensor.type == TELEM_TYPE_CALCULATED) {
      telemetryItems[i].eval(sensor);
    }
  }

#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
    varioWakeup();
  }
#endif

  static tmr10ms_t alarmsCheckTime = 0;
  #define SCHEDULE_NEXT_ALARMS_CHECK(seconds) alarmsCheckTime = get_tmr10ms() + (100*(seconds))
  if (int32_t(get_tmr10ms() - alarmsCheckTime) > 0) {

    SCHEDULE_NEXT_ALARMS_CHECK(1/*second*/);

    bool sensor_lost = false;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      if (isTelemetryFieldAvailable(i)) {
        TelemetryItem & item = telemetryItems[i];
        if (item.hasReceiveTime() && item.getDelaySinceLastValue() > TELEMETRY_VALUE_OLD_THRESHOLD) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->unit != UNIT_DATETIME) {
            item.setOld();
            sensor_lost = true;
          }
        }
      }
    }
    if (sensor_lost && TELEMETRY_STREAMING() &&  !g_model.rssiAlarms.disabled) {
      audioEvent(AU_SENSOR_LOST);
    }

#if defined(PCBTARANIS) || defined(PCBHORUS)
    if (isBadAntennaDetected()) {
      AUDIO_RAS_RED();
      POPUP_WARNING(STR_WARNING);
      const char * w = STR_ANTENNAPROBLEM;
      SET_WARNING_INFO(w, strlen(w), 0);
      SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
    }
#endif

    if (!g_model.rssiAlarms.disabled) {
      if (TELEMETRY_STREAMING()) {
        if (TELEMETRY_RSSI() < g_model.rssiAlarms.getCriticalRssi() ) {
          AUDIO_RSSI_RED();
          SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
        }
        else if (TELEMETRY_RSSI() < g_model.rssiAlarms.getWarningRssi() ) {
          AUDIO_RSSI_ORANGE();
          SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
        }
      }

      if (TELEMETRY_STREAMING()) {
        if (telemetryState == TELEMETRY_KO) {
          AUDIO_TELEMETRY_BACK();
        }
        telemetryState = TELEMETRY_OK;
      }
      else if (telemetryState == TELEMETRY_OK) {
        telemetryState = TELEMETRY_KO;
        AUDIO_TELEMETRY_LOST();
      }
    }
  }
}

void telemetryInterrupt10ms()
{

  if (TELEMETRY_STREAMING()) {
    if (!TELEMETRY_OPENXSENSOR()) {
      for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
        const TelemetrySensor & sensor = g_model.telemetrySensors[i];
        if (sensor.type == TELEM_TYPE_CALCULATED) {
          telemetryItems[i].per10ms(sensor);
        }
      }
    }

  }

#if defined(WS_HOW_HIGH)
  if (wshhStreaming > 0) {
    wshhStreaming--;
  }
#endif
  if (R9ModuleStreaming > 0) {
    R9ModuleStreaming--;
  }
  if (telemetryStreaming > 0) {
    telemetryStreaming--;
  }
  else {
#if !defined(SIMU)
    telemetryData.rssi.reset();
#endif
  }
}

void telemetryReset()
{
  memclear(&telemetryData, sizeof(telemetryData));

  for (int index=0; index<MAX_TELEMETRY_SENSORS; index++) {
    telemetryItems[index].clear();
  }

  telemetryStreaming = 0; // reset counter only if valid frsky packets are being detected

  telemetryState = TELEMETRY_INIT;
}

// we don't reset the telemetry here as we would also reset the consumption after model load
void telemetryInit(uint8_t protocol)
{
  telemetryProtocol = protocol;

  if (protocol == PROTOCOL_TELEMETRY_FRSKY_D) {
    telemetryPortInit(FRSKY_D_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
  }

#if defined(MULTIMODULE)
  else if (protocol == PROTOCOL_TELEMETRY_MULTIMODULE || protocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    // The DIY Multi module always speaks 100000 baud regardless of the telemetry protocol in use
    telemetryPortInit(MULTIMODULE_BAUDRATE, TELEMETRY_SERIAL_8E2);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
  }
  else if (protocol == PROTOCOL_TELEMETRY_SPEKTRUM) {
    // Spektrum's own small race RX (SPM4648) uses 125000 8N1, use the same since there is no real standard
    telemetryPortInit(125000, TELEMETRY_SERIAL_DEFAULT);
  }
#endif

#if defined(CROSSFIRE)
  else if (protocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
    telemetryPortInit(CROSSFIRE_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
    telemetryPortSetDirectionOutput();
  }
#endif

#if defined(AUX_SERIAL) || defined(PCBSKY9X)
  else if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    telemetryPortInit(0, TELEMETRY_SERIAL_DEFAULT);
    auxSerialTelemetryInit(PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY);
  }
#endif

  else {
    telemetryPortInit(FRSKY_SPORT_BAUDRATE, TELEMETRY_SERIAL_WITHOUT_DMA);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
  }

#if defined(REVX) && !defined(SIMU)
  if (serialInversion) {
    setMFP();
  }
  else {
    clearMFP();
  }
#endif
}


#if defined(LOG_TELEMETRY) && !defined(SIMU)
extern FIL g_telemetryFile;
void logTelemetryWriteStart()
{
  static tmr10ms_t lastTime = 0;
  tmr10ms_t newTime = get_tmr10ms();
  if (lastTime != newTime) {
    struct gtm utm;
    gettime(&utm);
    f_printf(&g_telemetryFile, "\r\n%4d-%02d-%02d,%02d:%02d:%02d.%02d0:", utm.tm_year+TM_YEAR_BASE, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100);
    lastTime = newTime;
  }
}

void logTelemetryWriteByte(uint8_t data)
{
  f_printf(&g_telemetryFile, " %02X", data);
}
#endif

OutputTelemetryBuffer outputTelemetryBuffer __DMA;

#if defined(LUA)
Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE> * luaInputTelemetryFifo = NULL;
#endif
