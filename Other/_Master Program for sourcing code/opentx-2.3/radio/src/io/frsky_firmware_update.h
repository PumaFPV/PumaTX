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

#ifndef _FRSKY_FIRMWARE_UPDATE_H_
#define _FRSKY_FIRMWARE_UPDATE_H_

#include "dataconstants.h"
#include "frsky_pxx2.h"

enum FrskyFirmwareProductFamily {
  FIRMWARE_FAMILY_INTERNAL_MODULE,
  FIRMWARE_FAMILY_EXTERNAL_MODULE,
  FIRMWARE_FAMILY_RECEIVER,
  FIRMWARE_FAMILY_SENSOR,
  FIRMWARE_FAMILY_BLUETOOTH_CHIP,
  FIRMWARE_FAMILY_POWER_MANAGEMENT_UNIT,
};

PACK(struct FrSkyFirmwareInformation {
  uint32_t fourcc;
  uint8_t headerVersion;
  uint8_t firmwareVersionMajor;
  uint8_t firmwareVersionMinor;
  uint8_t firmwareVersionRevision;
  uint32_t size;
  uint8_t productFamily;
  uint8_t productId;
  uint16_t crc;
});

const char * readFirmwareInformation(const char * filename, FrSkyFirmwareInformation & data);

class FrskyDeviceFirmwareUpdate {
    enum State {
      SPORT_IDLE,
      SPORT_POWERUP_REQ,
      SPORT_POWERUP_ACK,
      SPORT_VERSION_REQ,
      SPORT_VERSION_ACK,
      SPORT_DATA_TRANSFER,
      SPORT_DATA_REQ,
      SPORT_COMPLETE,
      SPORT_FAIL
    };

  public:
    FrskyDeviceFirmwareUpdate(ModuleIndex module):
      module(module) {
    }

    void flashFirmware(const char * filename);

  protected:
    uint8_t state = SPORT_IDLE;
    uint32_t address = 0;
    ModuleIndex module;
    uint8_t frame[12];

    void startup();

    void startFrame(uint8_t command);
    void sendFrame();

    const uint8_t * readFullDuplexFrame(ModuleFifo & fifo, uint32_t timeout);
    const uint8_t * readHalfDuplexFrame(uint32_t timeout);
    const uint8_t * readFrame(uint32_t timeout);
    bool waitState(State state, uint32_t timeout);
    void processFrame(const uint8_t * frame);

    const char * sendPowerOn();
    const char * sendReqVersion();
    const char * uploadFile(const char * filename);
    const char * endTransfer();
};

class FrskyChipFirmwareUpdate {
  public:
    FrskyChipFirmwareUpdate()
    {
    }

    void flashFirmware(const char * filename);

  protected:
    uint8_t crc;

    void sendByte(uint8_t byte, bool crc = true);
    const char * waitAnswer(uint8_t & status);
    const char * startBootloader();
    const char * sendUpgradeCommand(char command, uint32_t packetsCount);
    const char * sendUpgradeData(uint32_t index, uint8_t * data);

    const char * doFlashFirmware(const char * filename);
};

#endif // _FRSKY_FIRMWARE_UPDATE_H_
