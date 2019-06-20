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

#ifndef MODULEDATA_H
#define MODULEDATA_H

#include "constants.h"

#include <QtCore>

class Firmware;
class RadioDataConversionState;

enum PulsesProtocol {
  PULSES_OFF,
  PULSES_PPM,
  PULSES_SILV_A,
  PULSES_SILV_B,
  PULSES_SILV_C,
  PULSES_CTP1009,
  PULSES_LP45,
  PULSES_DSM2,
  PULSES_DSMX,
  PULSES_PPM16,
  PULSES_PPMSIM,
  PULSES_PXX_XJT_X16,
  PULSES_PXX_XJT_D8,
  PULSES_PXX_XJT_LR12,
  PULSES_PXX_DJT,
  PULSES_CROSSFIRE,
  PULSES_MULTIMODULE,
  PULSES_PXX_R9M,
  PULSES_PXX_R9M_LITE,
  PULSES_SBUS,
  PULSES_ACCESS_ISRM,
  PULSES_ACCESS_R9M,
  PULSES_ACCESS_R9M_LITE,
  PULSES_ACCESS_R9M_LITE_PRO,
  PULSES_PROTOCOL_LAST
};

enum MultiModuleRFProtocols {
  MODULE_SUBTYPE_MULTI_FLYSKY=0,
  MODULE_SUBTYPE_MULTI_FIRST=MODULE_SUBTYPE_MULTI_FLYSKY,
  MODULE_SUBTYPE_MULTI_HUBSAN,
  MODULE_SUBTYPE_MULTI_FRSKY,
  MODULE_SUBTYPE_MULTI_HISKY,
  MODULE_SUBTYPE_MULTI_V2X2,
  MODULE_SUBTYPE_MULTI_DSM2,
  MODULE_SUBTYPE_MULTI_DEVO,
  MODULE_SUBTYPE_MULTI_YD717,
  MODULE_SUBTYPE_MULTI_KN,
  MODULE_SUBTYPE_MULTI_SYMAX,
  MODULE_SUBTYPE_MULTI_SLT,
  MODULE_SUBTYPE_MULTI_CX10,
  MODULE_SUBTYPE_MULTI_CG023,
  MODULE_SUBTYPE_MULTI_BAYANG,
  MODULE_SUBTYPE_MULTI_ESky,
  MODULE_SUBTYPE_MULTI_MT99XX,
  MODULE_SUBTYPE_MULTI_MJXQ,
  MODULE_SUBTYPE_MULTI_SHENQI,
  MODULE_SUBTYPE_MULTI_FY326,
  MODULE_SUBTYPE_MULTI_SFHSS,
  MODULE_SUBTYPE_MULTI_J6PRO,
  MODULE_SUBTYPE_MULTI_FQ777,
  MODULE_SUBTYPE_MULTI_ASSAN,
  MODULE_SUBTYPE_MULTI_HONTAI,
  MODULE_SUBTYPE_MULTI_OLRS,
  MODULE_SUBTYPE_MULTI_FS_AFHDS2A,
  MODULE_SUBTYPE_MULTI_Q2X2,
  MODULE_SUBTYPE_MULTI_WK_2X01,
  MODULE_SUBTYPE_MULTI_Q303,
  MODULE_SUBTYPE_MULTI_GW008,
  MODULE_SUBTYPE_MULTI_DM002,
  MODULE_SUBTYPE_MULTI_CABELL,
  MODULE_SUBTYPE_MULTI_ESKY150,
  MODULE_SUBTYPE_MULTI_H83D,
  MODULE_SUBTYPE_MULTI_CORONA,
  MODULE_SUBTYPE_MULTI_CFLIE,
  MODULE_SUBTYPE_MULTI_HITEC,
  MODULE_SUBTYPE_MULTI_WFLY,
  MODULE_SUBTYPE_MULTI_BUGS,
  MODULE_SUBTYPE_MULTI_BUGS_MINI,
  MODULE_SUBTYPE_MULTI_TRAXXAS,
  MODULE_SUBTYPE_MULTI_NCC1701,
  MODULE_SUBTYPE_MULTI_E01X,
  MODULE_SUBTYPE_MULTI_V911S,
  MODULE_SUBTYPE_MULTI_GD00X,
  MODULE_SUBTYPE_MULTI_V761,
  MODULE_SUBTYPE_MULTI_LAST = MODULE_SUBTYPE_MULTI_V761
};

enum TrainerProtocol {
  TRAINER_MASTER_JACK,
  TRAINER_SLAVE_JACK,
  TRAINER_MASTER_SBUS_MODULE,
  TRAINER_MASTER_CPPM_MODULE,
  TRAINER_MASTER_SBUS_BATT_COMPARTMENT
};

enum ModuleSubtypeR9M {
  MODULE_SUBTYPE_R9M_FCC,
  MODULE_SUBTYPE_R9M_EU,
  MODULE_SUBTYPE_R9M_EUPLUS,
  MODULE_SUBTYPE_R9M_AUPLUS,
  MODULE_SUBTYPE_R9M_LAST=MODULE_SUBTYPE_R9M_AUPLUS
};

class ModuleData {
  Q_DECLARE_TR_FUNCTIONS(ModuleData)

  public:
    ModuleData() { clear(); }
    unsigned int modelId;
    int          protocol;
    unsigned int subType;
    bool         invertedSerial;
    unsigned int channelsStart;
    int          channelsCount; // 0=8 channels
    unsigned int failsafeMode;
    int          failsafeChannels[CPN_MAX_CHNOUT];

    struct {
      int delay;
      bool pulsePol;           // false = positive
      bool outputType;         // false = open drain, true = push pull
      int frameLength;
    } ppm;

    struct {
      unsigned int rfProtocol;
      bool autoBindMode;
      bool lowPowerMode;
      bool customProto;
      int optionValue;
    } multi;

    struct {
      int power;                   // 0 10 mW, 1 100 mW, 2 500 mW, 3 1W
      bool receiver_telem_off;     // false = receiver telem enabled
      bool receiver_channel_9_16;  // false = pwm out 1-8, true 9-16
      bool external_antenna;       // false = internal antenna, true = external antenna
      bool sport_out;
    } pxx;

    struct {
      // unsigned int receivers;
      // char         receiverName[3][8];
      char data[1 + 3 * 8];
    } access;

    void clear() { memset(this, 0, sizeof(ModuleData)); }
    void convert(RadioDataConversionState & cstate);
    QString polarityToString() const { return ppm.pulsePol ? tr("Positive") : tr("Negative"); }
    QString rfProtocolToString() const;
    QString subTypeToString(int type = -1) const;
    QString powerValueToString(Firmware * fw) const;
    static QString indexToString(int index, Firmware * fw);
    static QString protocolToString(unsigned protocol);
    static QStringList powerValueStrings(int subType, Firmware * fw);
};

#endif // MODULEDATA_H
