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
#include "opentx.h"

class Pxx2Telemetry
{
  protected:
    static void processGetHardwareInfoFrame(uint8_t module, uint8_t *frame);

    static void processReceiverSettingsFrame(uint8_t module, uint8_t *frame);

    static void processRegisterFrame(uint8_t module, uint8_t * frame);

    static void processBindFrame(uint8_t module, uint8_t * frame);

    static void processTelemetryFrame(uint8_t module, uint8_t * frame);

    static void processSpectrumFrame(uint8_t module, uint8_t * frame);

    static void processRadioFrame(uint8_t module, uint8_t * frame);

    static void processPowerMeterFrame(uint8_t module, uint8_t * frame);

  public:
};

void processGetHardwareInfoFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_GET_HARDWARE_INFO) {
    return;
  }

  ModuleInformation * destination = moduleState[module].moduleInformation;

  uint8_t index = frame[3];
  uint8_t modelId = frame[4];
  if (index == PXX2_HW_INFO_TX_ID && modelId < DIM(PXX2modulesModels)) {
    memcpy(&destination->information, &frame[4], sizeof(PXX2HardwareInformation));
  }
  else if (index < PXX2_MAX_RECEIVERS_PER_MODULE && modelId < DIM(PXX2receiversModels)) {
    memcpy(&destination->receivers[index].information, &frame[4], sizeof(PXX2HardwareInformation));
    destination->receivers[index].timestamp = get_tmr10ms();
  }
}

void processModuleSettingsFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_MODULE_SETTINGS) {
    return;
  }

  ModuleSettings * destination = moduleState[module].moduleSettings;

  // Flag1
  if (frame[4] & PXX2_TX_SETTINGS_FLAG1_EXTERNAL_ANTENNA)
    destination->externalAntenna = 1;

  // Power
  destination->txPower = frame[5];

  destination->state = PXX2_SETTINGS_OK;
  destination->retryTime = 0;
  moduleState[module].mode = MODULE_MODE_NORMAL;
}

void processReceiverSettingsFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_RECEIVER_SETTINGS) {
    return;
  }

  if (frame[4] & PXX2_RX_SETTINGS_FLAG1_FASTPWM)
    reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate = 1;

  if (frame[4] & PXX2_RX_SETTINGS_FLAG1_TELEMETRY_DISABLED)
    reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled = 1;

  uint8_t outputsCount = min<uint8_t>(16, frame[0] - 4);
  reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount = outputsCount;
  for (uint8_t pin = 0; pin < outputsCount; pin++) {
    reusableBuffer.hardwareAndSettings.receiverSettings.outputsMapping[pin] = frame[5 + pin];
  }

  reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_OK;
  reusableBuffer.hardwareAndSettings.receiverSettings.timeout = 0;
  moduleState[module].mode = MODULE_MODE_NORMAL;
}

void processRegisterFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_REGISTER) {
    return;
  }

  switch(frame[3]) {
    case 0x00:
      if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_INIT) {
        // RX_NAME follows, we store it for the next step
        str2zchar(reusableBuffer.moduleSetup.pxx2.registerRxName, (const char *)&frame[4], PXX2_LEN_RX_NAME);
        reusableBuffer.moduleSetup.pxx2.registerLoopIndex = frame[12];
        reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_RX_NAME_RECEIVED;
      }
      break;

    case 0x01:
      if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_RX_NAME_SELECTED) {
        // RX_NAME + PASSWORD follow, we check they are good
        if (cmpStrWithZchar((char *)&frame[4], reusableBuffer.moduleSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME) &&
            cmpStrWithZchar((char *)&frame[12], g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID)) {
          reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_OK;
          moduleState[module].mode = MODULE_MODE_NORMAL;
          POPUP_INFORMATION(STR_REG_OK);
        }
      }
      break;
  }
}

void processBindFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_BIND) {
    return;
  }

  BindInformation * destination = moduleState[module].bindInformation;

  switch(frame[3]) {
    case 0x00:
      if (destination->step == BIND_INIT) {
        bool found = false;
        for (uint8_t i=0; i<destination->candidateReceiversCount; i++) {
          if (memcmp(destination->candidateReceiversNames[i], &frame[4], PXX2_LEN_RX_NAME) == 0) {
            found = true;
            break;
          }
        }
        if (!found && destination->candidateReceiversCount < PXX2_MAX_RECEIVERS_PER_MODULE) {
          memcpy(destination->candidateReceiversNames[destination->candidateReceiversCount++], &frame[4], PXX2_LEN_RX_NAME);
          if (moduleState[module].callback) {
            moduleState[module].callback();
          }
        }
      }
      break;

    case 0x01:
      if (destination->step == BIND_START) {
        if (memcmp(&destination->candidateReceiversNames[destination->selectedReceiverIndex], &frame[4], PXX2_LEN_RX_NAME) == 0) {
          memcpy(g_model.moduleData[module].pxx2.receiverName[destination->rxUid], &frame[4], PXX2_LEN_RX_NAME);
          storageDirty(EE_MODEL);
          destination->step = BIND_WAIT;
          destination->timeout = get_tmr10ms() + 30;
        }
      }
      break;

    case 0x02:
      if (destination->step == BIND_INFO_REQUEST) {
        if (memcmp(&destination->candidateReceiversNames[destination->selectedReceiverIndex], &frame[4], PXX2_LEN_RX_NAME) == 0) {
          memcpy(&destination->receiverInformation, &frame[12], sizeof(PXX2HardwareInformation));
          if (moduleState[module].callback) {
            moduleState[module].callback();
          }
        }
      }
      break;
  }
}

void processResetFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_RESET) {
    return;
  }

  if (reusableBuffer.moduleSetup.pxx2.resetReceiverIndex == frame[3]) {
    memclear(g_model.moduleData[module].pxx2.receiverName[reusableBuffer.moduleSetup.pxx2.resetReceiverIndex], PXX2_LEN_RX_NAME);
  }

  moduleState[module].mode = MODULE_MODE_NORMAL;
}

void processTelemetryFrame(uint8_t module, uint8_t * frame)
{
  uint8_t origin = (module << 2) + (frame[3] & 0x03);
  sportProcessTelemetryPacketWithoutCrc(origin, &frame[4]);
}

void processSpectrumAnalyserFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    return;
  }

  uint32_t * frequency = (uint32_t *)&frame[4];
  int8_t * power = (int8_t *)&frame[8];

  // center = 2440000000;  // 2440MHz
  // span = 40000000;  // 40MHz
  // left = 2440000000 - 20000000
  // step = 10000

  // TRACE("Fq=%u, Pw=%d, X=%d, Y=%d", *frequency, int32_t(*power), D * 128 / 40000000, int32_t(127 + *power));

  int32_t position = *frequency - (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2);
  uint32_t x = (position * LCD_W / 8) / (reusableBuffer.spectrumAnalyser.span / 8);
  if (x < LCD_W) {
    reusableBuffer.spectrumAnalyser.bars[x] = 0x80 + *power;
  }
}

void processPowerMeterFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_POWER_METER) {
    return;
  }

  reusableBuffer.powerMeter.power = *((int16_t *)&frame[8]);
  if (!reusableBuffer.powerMeter.peak || reusableBuffer.powerMeter.power > reusableBuffer.powerMeter.peak) {
    reusableBuffer.powerMeter.peak = reusableBuffer.powerMeter.power;
  }
}

void processOtaUpdateFrame(uint8_t module, uint8_t * frame)
{
  if (moduleState[module].mode != MODULE_MODE_OTA_UPDATE) {
    return;
  }

  OtaUpdateInformation * destination = moduleState[module].otaUpdateInformation;

  if (destination->step == OTA_UPDATE_START) {
    if (frame[3] == 0x00 && memcmp(destination->candidateReceiversNames[destination->selectedReceiverIndex], &frame[4], PXX2_LEN_RX_NAME) == 0) {
      destination->step = OTA_UPDATE_START_ACK;
    }
  }
  else if (destination->step == OTA_UPDATE_TRANSFER) {
    uint32_t address = *((uint32_t *)&frame[4]);
    if (frame[3] == 0x01 && destination->address == address) {
      destination->step = OTA_UPDATE_TRANSFER_ACK;
    }
  }
  else if (destination->step == OTA_UPDATE_EOF) {
    if (frame[3] == 0x02) {
      destination->step = OTA_UPDATE_EOF_ACK;
    }
  }
}

void processModuleFrame(uint8_t module, uint8_t *frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_HW_INFO:
      processGetHardwareInfoFrame(module, frame);
      break;

    case PXX2_TYPE_ID_TX_SETTINGS:
      processModuleSettingsFrame(module, frame);
      break;

    case PXX2_TYPE_ID_RX_SETTINGS:
      processReceiverSettingsFrame(module, frame);
      break;

    case PXX2_TYPE_ID_REGISTER:
      processRegisterFrame(module, frame);
      break;

    case PXX2_TYPE_ID_BIND:
      processBindFrame(module, frame);
      break;

    case PXX2_TYPE_ID_TELEMETRY:
      processTelemetryFrame(module, frame);
      break;

    case PXX2_TYPE_ID_RESET:
      processResetFrame(module, frame);
      break;
  }
}

void processToolsFrame(uint8_t module, uint8_t * frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_POWER_METER:
      processPowerMeterFrame(module, frame);
      break;

    case PXX2_TYPE_ID_SPECTRUM:
      processSpectrumAnalyserFrame(module, frame);
      break;
  }
}

void processPXX2Frame(uint8_t module, uint8_t *frame)
{
  switch (frame[1]) {
    case PXX2_TYPE_C_MODULE:
      processModuleFrame(module, frame);
      break;

    case PXX2_TYPE_C_POWER_METER:
      processToolsFrame(module, frame);
      break;

    case PXX2_TYPE_C_OTA:
      processOtaUpdateFrame(module, frame);
      break;

    default:
      break;
  }
}
