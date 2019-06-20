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

#include "boards.h"
#include "helpers.h"
#include "opentxeeprom.h"
#include "customdebug.h"
#include <stdlib.h>
#include <algorithm>

using namespace Board;

#define MAX_VIEWS(board)                      (HAS_LARGE_LCD(board) ? 2 : 256)
#define MAX_SLIDERS(board)                    (IS_HORUS_X10(board) ? 4 : (Boards::getCapability(board, Board::Sliders))) //TODO need to be remove when x10 eeprom gets fixed
#define MAX_MOUSE_ANALOGS(board)              (IS_HORUS_X10(board) ? 2 : (Boards::getCapability(board, Board::MouseAnalogs))) //TODO need to be remove when x10 eeprom gets fixed
#define MAX_GYRO_ANALOGS(board, version)      (version >= 219 ? Boards::getCapability(board, Board::GyroAnalogs) : 0)
#define MAX_SWITCHES(board, version)          (version <= 218 && IS_TARANIS_X7(board) ? 6 : Boards::getCapability(board, Board::Switches))
#define MAX_SWITCH_SLOTS(board, version)      (IS_TARANIS_X9E(board) ? 32 : 8)  // bitsize of swconfig_t / 2 (see radio/src/datastructs.h)
#define MAX_SWITCHES_POSITION(board, version) (Boards::getCapability(board, Board::SwitchPositions))
#define MAX_ROTARY_ENCODERS(board)            (IS_SKY9X(board) ? 1 : 0)
#define MAX_FLIGHT_MODES(board, version)      9
#define MAX_TIMERS(board, version)            3
#define MAX_MIXERS(board, version)            64
#define MAX_CHANNELS(board, version)          32
#define MAX_TRIMS(board)                      (Boards::getCapability(board, Board::NumTrims))
#define MAX_EXPOS(board, version)             (IS_HORUS_OR_TARANIS(board) ? 64 : 32)
#define MAX_LOGICAL_SWITCHES(board, version)  (version >= 218 ? 64 : 32)
#define MAX_CUSTOM_FUNCTIONS(board, version)  64
#define MAX_CURVES(board, version)            ((version >= 219 || HAS_LARGE_LCD(board)) ? 32 : 16)
#define MAX_GVARS(board, version)             9
#define MAX_SCRIPTS(board)                    (IS_HORUS(board) ? 9 : 7)
#define MAX_TELEMETRY_SENSORS(board, version) (version <= 218 ? 32 : ((IS_HORUS(board) || IS_TARANIS_X9(board)) ? 60 : 40))
#define NUM_PPM_INPUTS(board, version)        16
#define ROTENC_COUNT(board, version)          ((IS_STM32(board) && version >= 218) ? 0 : 1)
#define MAX_AUX_TRIMS(board)                  (IS_HORUS(board) ? 2 : 0)

inline int switchIndex(int i, Board::Type board, unsigned int version)
{
  if (!IS_HORUS_OR_TARANIS(board))
    return (i<=3 ? i+3 : (i<=6 ? i-3 : i));
  else
    return i;
}

class SwitchesConversionTable: public ConversionTable {

  public:
    SwitchesConversionTable(Board::Type board, unsigned int version, bool timer=false)
    {
      int val=0;
      int offset=0;
      if (timer) {
        offset = 4;
        for (int i=0; i<5; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_TIMER_MODE, i), val++);
        }
      }
      else {
        addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);
      }

      for (int i=1; i<=MAX_SWITCHES_POSITION(board, version); i++) {
        int s = switchIndex(i, board, version);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val+offset);
        val++;
      }

      if (IS_HORUS_OR_TARANIS(board)) {
        for (int i=1; i<=Boards::getCapability(board, Board::Pots)*6; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i), val++);
        }
      }

      for (int i=1; i<=2*MAX_TRIMS(board); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_TRIM, -i), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_TRIM, i), val++);
      }

      for (int i=1; i<=MAX_ROTARY_ENCODERS(board); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, -i), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i), val++);
      }

      for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_OFF), -val+offset);
      addConversion(RawSwitch(SWITCH_TYPE_ON), val++);

      addConversion(RawSwitch(SWITCH_TYPE_ONE, -1), -val+offset);
      addConversion(RawSwitch(SWITCH_TYPE_ONE, 1), val++);
      for (int i=1; i<=MAX_FLIGHT_MODES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_FLIGHT_MODE, -i), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i), val++);
      }

      if (version >= 218) {
        addConversion(RawSwitch(SWITCH_TYPE_TELEMETRY, -1), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_TELEMETRY, 1), val++);
        for (int i=1; i<=MAX_TELEMETRY_SENSORS(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_SENSOR, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_SENSOR, i), val++);
        }
      }
    }


  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }

    class Cache {
      public:
        Cache(Board::Type board, unsigned int version, unsigned long flags, SwitchesConversionTable * table):
          board(board),
          version(version),
          flags(flags),
          table(table)
        {
        }
        Board::Type board;
        unsigned int version;
        unsigned long flags;
        SwitchesConversionTable * table;
    };

    static std::list<Cache> internalCache;

  public:

    static SwitchesConversionTable * getInstance(Board::Type board, unsigned int version, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.board == board && element.version == version && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, flags, new SwitchesConversionTable(board, version, flags));
      internalCache.push_back(element);
      return element.table;
    }
    static void Cleanup()
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.table)
          delete element.table;
      }
      internalCache.clear();
    }
};

std::list<SwitchesConversionTable::Cache> SwitchesConversionTable::internalCache;

#define FLAG_NONONE       0x01
#define FLAG_NOSWITCHES   0x02
#define FLAG_NOTELEMETRY  0x04

class SourcesConversionTable: public ConversionTable {

  public:
    SourcesConversionTable(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      int val=0;

      if (!(flags & FLAG_NONONE)) {
        addConversion(RawSource(SOURCE_TYPE_NONE), val++);
      }

      if (version >= 218 || IS_STM32(board)) {
        for (int i=0; i<32; i++) {
          addConversion(RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i), val++);
        }
      }

      if (IS_STM32(board)) {
        for (int i = 0; i < MAX_SCRIPTS(board); i++) {
          for (int j = 0; j < 6; j++) {
            addConversion(RawSource(SOURCE_TYPE_LUA_OUTPUT, i * 16 + j), val++);
          }
        }
      }

      for (int i=0; i<CPN_MAX_STICKS+Boards::getCapability(board, Board::Pots)+MAX_SLIDERS(board)+MAX_MOUSE_ANALOGS(board)+MAX_GYRO_ANALOGS(board, version); i++) {
        addConversion(RawSource(SOURCE_TYPE_STICK, i), val++);
      }

      for (int i=0; i<MAX_ROTARY_ENCODERS(board); i++) {
        addConversion(RawSource(SOURCE_TYPE_ROTARY_ENCODER, 0), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_MAX), val++);

      for (int i=0; i<3; i++)
        addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);

      for (int i=0; i<MAX_TRIMS(board); i++)
        addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);

      addConversion(RawSource(SOURCE_TYPE_SWITCH, 0), val++);

      if (!(flags & FLAG_NOSWITCHES)) {
        for (int i=1; i<MAX_SWITCHES(board, version); i++)
          addConversion(RawSource(SOURCE_TYPE_SWITCH, i), val++);
        for (int i=0; i<MAX_LOGICAL_SWITCHES(board, version); i++)
          addConversion(RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i), val++);
      }

      for (int i=0; i<NUM_PPM_INPUTS(board, version); i++) {
        addConversion(RawSource(SOURCE_TYPE_PPM, i), val++);
      }

      for (int i=0; i<MAX_CHANNELS(board, version); i++) {
        addConversion(RawSource(SOURCE_TYPE_CH, i), val++);
      }

      if (!(flags & FLAG_NOTELEMETRY)) {
        for (int i=0; i<MAX_GVARS(board, version); i++)
          addConversion(RawSource(SOURCE_TYPE_GVAR, i), val++);
        addConversion(RawSource(SOURCE_TYPE_SPECIAL, 0), val++); // BATT
        addConversion(RawSource(SOURCE_TYPE_SPECIAL, 1), val++); // TIME
        val += 5;
        addConversion(RawSource(SOURCE_TYPE_SPECIAL, 2), val++); // Timer1
        addConversion(RawSource(SOURCE_TYPE_SPECIAL, 3), val++); // Timer2
        addConversion(RawSource(SOURCE_TYPE_SPECIAL, 4), val++); // Timer3
        for (unsigned i=0; i<MAX_TELEMETRY_SENSORS(board, version)*3; ++i) {
          addConversion(RawSource(SOURCE_TYPE_TELEMETRY, i), val++);
        }
      }
    }

  protected:

    void addConversion(const RawSource & source, const int b)
    {
      ConversionTable::addConversion(source.toValue(), b);
    }

    class Cache {
      public:
        Cache(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags, SourcesConversionTable * table):
          board(board),
          version(version),
          variant(variant),
          flags(flags),
          table(table)
        {
        }
        Board::Type board;
        unsigned int version;
        unsigned int variant;
        unsigned long flags;
        SourcesConversionTable * table;
    };
    static std::list<Cache> internalCache;

  public:

    static SourcesConversionTable * getInstance(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.board == board && element.version == version && element.variant == variant && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, variant, flags, new SourcesConversionTable(board, version, variant, flags));
      internalCache.push_back(element);
      return element.table;
    }
    static void Cleanup()
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.table)
          delete element.table;
      }
      internalCache.clear();
    }
};

std::list<SourcesConversionTable::Cache> SourcesConversionTable::internalCache;

void OpenTxEepromCleanup(void)
{
  SourcesConversionTable::Cleanup();
  SwitchesConversionTable::Cleanup();
}

template <int N>
class SwitchField: public ConversionField< SignedField<N> > {
  public:
    SwitchField(DataField * parent, RawSwitch & sw, Board::Type board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<N> >(parent, _switch, SwitchesConversionTable::getInstance(board, version, flags), DataField::tr("Switch").toLatin1(),
          DataField::tr("Switch ").toLatin1() + sw.toString(board) + DataField::tr(" cannot be exported on this board!").toLatin1()),
      sw(sw),
      _switch(0),
      board(board)
    {
    }

    virtual ~SwitchField()
    {
    }

    virtual void beforeExport()
    {
      _switch = sw.toValue();
      ConversionField< SignedField<N> >::beforeExport();
    }

    virtual void afterImport()
    {
      ConversionField< SignedField<N> >::afterImport();
      sw = RawSwitch(_switch);
      qCDebug(eepromImport) << QString("imported %1: %2").arg(ConversionField< SignedField<N> >::internalField.getName()).arg(sw.toString(board));
    }

  protected:
    RawSwitch & sw;
    int _switch;
    Board::Type board;
};

template <int N>
class SourceField: public ConversionField< UnsignedField<N> > {
  public:
    SourceField(DataField * parent, RawSource & source, Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0):
      ConversionField< UnsignedField<N> >(parent, _source, SourcesConversionTable::getInstance(board, version, variant, flags),
            DataField::tr("Source").toLatin1(), DataField::tr("Source %1 cannot be exported on this board!").arg(source.toString())),
      source(source),
      _source(0)
    {
    }

    virtual ~SourceField()
    {
    }

    virtual void beforeExport()
    {
      _source = source.toValue();
      ConversionField< UnsignedField<N> >::beforeExport();
    }

    virtual void afterImport()
    {
      ConversionField< UnsignedField<N> >::afterImport();
      source = RawSource(_source);
      qCDebug(eepromImport) << QString("imported %1: %2").arg(ConversionField< UnsignedField<N> >::internalField.getName()).arg(source.toString());
    }

  protected:
    RawSource & source;
    unsigned int _source;
};


int smallGvarImport(int gvar)
{
  if (gvar < -10000) {
    gvar = 128 + gvar + 10000;
  }
  else if (gvar > 10000) {
    gvar = -128 +gvar - 10001;
  }
  return gvar;
}

int smallGvarExport(int gvar)
{
  if (gvar > 110) {
    gvar = gvar - 128 - 10000;
  }
  else if (gvar < -110) {
    gvar = gvar + 128 + 10001;
  }
  return gvar;
}

void splitGvarParam(const int gvar, int & _gvar, unsigned int & _gvarParam, const Board::Type board, const int version)
{
  if (gvar < -10000) {
    _gvarParam = 0;
    _gvar = 256 + gvar + 10000;
  }
  else if (gvar > 10000) {
    _gvarParam = 1;
    _gvar = gvar - 10001;
  }
  else {
    if (gvar < 0) _gvarParam = 1;
    else          _gvarParam = 0;
    _gvar = gvar;  // save routine throws away all unused bits; therefore no 2er complement compensation needed here
  }
}

void concatGvarParam(int & gvar, const int _gvar, const unsigned int _gvarParam, const Board::Type board, const int version)
{
  gvar = _gvar;
  if (gvar<0) gvar+=256;  // remove 2er complement, because 8bit part is in this case unsigned
  if (_gvarParam) {  // here is the real sign bit
    gvar|=-256;   // set all higher bits to simulate negative value
  }

  if (gvar>245) {
    gvar = gvar-256 - 10000;
  }
  else if (gvar<-245) {
    gvar = gvar+256 + 10001;
  }

}

void exportGvarParam(const int gvar, int & _gvar, int version)
{
  int GV1 = 1024;

  if (gvar < -10000) {
    _gvar = GV1 + gvar + 10000;
  }
  else if (gvar > 10000) {
    _gvar = GV1 + gvar - 10001;
  }
  else {
    _gvar = gvar;
  }
}

void importGvarParam(int & gvar, const int _gvar, int version)
{
  int GV1 = 1024;

  if (_gvar >= GV1) {
    gvar = 10001 + _gvar - GV1;
  }
  else if (_gvar >= GV1-9) {
    gvar = -10000 + _gvar - GV1;
  }
  else if (_gvar < -GV1) {
    gvar = -10000 + _gvar + GV1 + 1;
  }
  else if (_gvar < -GV1+9) {
    gvar = 10000 + _gvar + GV1 + 1;
  }
  else {
    gvar = _gvar;
  }

  // qDebug() << QString("import") << _gvar << gvar;
}

class CurveReferenceField: public TransformedField {
  public:
    CurveReferenceField(DataField * parent, CurveReference & curve, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this),
      curve(curve),
      _curve_type(0),
      _curve_value(0)
    {
      internalField.Append(new UnsignedField<8>(this, _curve_type));
      internalField.Append(new SignedField<8>(this, _curve_value));
    }

    virtual void beforeExport()
    {
      if (curve.value != 0) {
        _curve_type = (unsigned int)curve.type;
        _curve_value = smallGvarImport(curve.value);
      }
      else {
        _curve_type = 0;
        _curve_value = 0;
      }
    }

    virtual void afterImport()
    {
      curve.type = (CurveReference::CurveRefType)_curve_type;
      curve.value = smallGvarExport(_curve_value);
      qCDebug(eepromImport) << QString("imported CurveReference(%1)").arg(curve.toString());
    }

  protected:
    StructField internalField;
    CurveReference & curve;
    unsigned int _curve_type;
    int _curve_value;
};

static int exportHeliInversionWeight(int source) {
  return (source < 0) ? 1 : 0;
}

static int importHeliInversionWeight(int source) {
  return source ? -100: 100;
}

class HeliField: public StructField {
  public:
    HeliField(DataField * parent, SwashRingData & heli, Board::Type board, unsigned int version, unsigned int variant):
      StructField(parent, "Heli")
    {
      if (version >= 218 || IS_STM32(board)) {
        Append(new UnsignedField<8>(this, heli.type));
        Append(new UnsignedField<8>(this, heli.value));
        Append(new SourceField<8>(this, heli.collectiveSource, board, version, variant));
        Append(new SourceField<8>(this, heli.aileronSource, board, version, variant));
        Append(new SourceField<8>(this, heli.elevatorSource, board, version, variant));
        Append(new SignedField<8>(this, heli.collectiveWeight));
        Append(new SignedField<8>(this, heli.aileronWeight));
        Append(new SignedField<8>(this, heli.elevatorWeight));
      }
      else {
        Append(new ConversionField< SignedField<1> >(this, heli.elevatorWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(this, heli.aileronWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(this, heli.collectiveWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new UnsignedField<5>(this, heli.type));
        Append(new SourceField<8>(this, heli.collectiveSource, board, version, variant));
        //, FLAG_NOSWITCHES)); Fix shift in collective
        Append(new UnsignedField<8>(this, heli.value));
      }
    }
};

class FlightModeField: public TransformedField {
  public:
    FlightModeField(DataField * parent, FlightModeData & phase, int index, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "FlightMode"),
      phase(phase),
      index(index),
      board(board),
      version(version),
      rotencCount(ROTENC_COUNT(board, version))
    {
      if (version >= 218) {
        for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
          internalField.Append(new SignedField<11>(this, phase.trim[i]));
          internalField.Append(new UnsignedField<5>(this, trimMode[i]));
        }
      }
      else if (IS_TARANIS(board)) {
        for (int i=0; i<CPN_MAX_STICKS; i++) {
          internalField.Append(new SignedField<11>(this, phase.trim[i]));
          internalField.Append(new UnsignedField<5>(this, trimMode[i]));
        }
      }
      else {
        for (int i=0; i<CPN_MAX_STICKS; i++) {
          internalField.Append(new SignedField<16>(this, trimBase[i]));
        }
      }
      if (version >= 218) {
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(this, phase.name, "Flight mode name"));
        else
          internalField.Append(new ZCharField<6>(this, phase.name, "Flight mode name"));
        internalField.Append(new SwitchField<9>(this, phase.swtch, board, version));
        internalField.Append(new SpareBitsField<7>(this));
      }
      else {
        internalField.Append(new SwitchField<8>(this, phase.swtch, board, version));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(this, phase.name));
        else
          internalField.Append(new ZCharField<6>(this, phase.name));
      }
      internalField.Append(new UnsignedField<8>(this, phase.fadeIn));
      internalField.Append(new UnsignedField<8>(this, phase.fadeOut));

      for (int i=0; i<rotencCount; i++) {
        internalField.Append(new SignedField<16>(this, phase.rotaryEncoders[i]));
      }

      for (int i=0; i<MAX_GVARS(board, version); i++) {
        internalField.Append(new SignedField<16>(this, phase.gvars[i]));
      }
    }

    virtual void beforeExport()
    {
      for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
        if (IS_HORUS_OR_TARANIS(board) || version >= 218) {
          if (phase.trimMode[i] < 0)
            trimMode[i] = TRIM_MODE_NONE;
          else
            trimMode[i] = 2*phase.trimRef[i] + phase.trimMode[i];
        }
        else {
          int trim;
          if (phase.trimMode[i] < 0)
            trim = 0;
          else if (phase.trimRef[i] != index)
            trim = 501 + phase.trimRef[i] - (phase.trimRef[i] > index ? 1 : 0);
          else
            trim = std::max(-500, std::min(500, phase.trim[i]));
          if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215)) {
            trimBase[i] = trim >> 2;
            trimExt[i] = (trim & 0x03);
          }
          else {
            trimBase[i] = trim;
          }
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
        if (IS_HORUS_OR_TARANIS(board) || version >= 218) {
          if (trimMode[i] == TRIM_MODE_NONE) {
            phase.trimMode[i] = -1;
          }
          else {
            phase.trimMode[i] = trimMode[i] % 2;
            phase.trimRef[i] = trimMode[i] / 2;
          }
        }
        else {
          if (phase.swtch == RawSwitch(SWITCH_TYPE_NONE) && index > 0) {
            phase.trimRef[i] = 0;
            phase.trimMode[i] = 0;
            phase.trim[i] = 0;
          }
          else {
            int trim;
            if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215))
              trim = ((trimBase[i]) << 2) + (trimExt[i] & 0x03);
            else
              trim = trimBase[i];
            if (trim > 500) {
              phase.trimRef[i] = trim - 501;
              if (phase.trimRef[i] >= index)
                phase.trimRef[i] += 1;
              phase.trimMode[i] = 0;
              phase.trim[i] = 0;
            }
            else {
              phase.trimRef[i] = index/*own trim*/;
              phase.trimMode[i] = 0;
              phase.trim[i] = trim;
            }
          }
        }
      }
      qCDebug(eepromImport) << QString("imported %1: '%2'").arg(internalField.getName()).arg(phase.name);
    }

  protected:
    StructField internalField;
    FlightModeData & phase;
    int index;
    Board::Type board;
    unsigned int version;
    int rotencCount;
    int trimBase[CPN_MAX_TRIMS];
    int trimExt[CPN_MAX_TRIMS];
    unsigned int trimMode[CPN_MAX_TRIMS];
};

class MixField: public TransformedField {
  public:
    MixField(DataField * parent, MixData & mix, Board::Type board, unsigned int version, ModelData * model):
      TransformedField(parent, internalField),
      internalField(this, "Mix"),
      mix(mix),
      board(board),
      version(version),
      model(model)
    {
      if (version >= 218) {
        internalField.Append(new SignedField<11>(this, _weight));
        internalField.Append(new UnsignedField<5>(this, _destCh));
        internalField.Append(new SourceField<10>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<1>(this, (unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>(this));
        internalField.Append(new SignedField<14>(this, _offset));
        internalField.Append(new SwitchField<9>(this, mix.swtch, board, version));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new CurveReferenceField(this, mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        if (IS_HORUS(board))
          internalField.Append(new ZCharField<6>(this, mix.name));
        else if (HAS_LARGE_LCD(board) && version <= 218)
          internalField.Append(new ZCharField<8>(this, mix.name));
        else
          internalField.Append(new ZCharField<6>(this, mix.name));
      }
      else if (IS_TARANIS(board)) {
        internalField.Append(new UnsignedField<8>(this, _destCh));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<1>(this, (unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<4>(this, mix.mixWarn));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SourceField<10>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<14>(this, _offset));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new CurveReferenceField(this, mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new ZCharField<8>(this, mix.name));
      }
      else {
        internalField.Append(new UnsignedField<5>(this, _destCh));
        internalField.Append(new UnsignedField<3>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _curveParam));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(this, _offset));
        internalField.Append(new ZCharField<6>(this, mix.name));
      }
    }

    virtual void beforeExport()
    {
      if (mix.destCh && mix.srcRaw.type != SOURCE_TYPE_NONE) {
        _destCh = mix.destCh - 1;
        if (mix.curve.type == CurveReference::CURVE_REF_CUSTOM) {
          _curveMode = true;
          if (mix.curve.value > 0) {
            _curveParam = 6 + mix.curve.value;
          }
          else {
            _curveParam = mix.curve.value;
          }
        }
        else if (mix.curve.type == CurveReference::CURVE_REF_FUNC) {
          _curveMode = true;
          _curveParam = mix.curve.value;
        }
        else if (mix.curve.type == CurveReference::CURVE_REF_DIFF) {
          _curveMode = 0;
          _curveParam = smallGvarImport(mix.curve.value);
        }
      }
      else {
        mix.clear();
        _destCh = 0;
        _curveMode = 0;
        _curveParam = 0;
      }

      exportGvarParam(mix.weight, _weight, version);
      if (version >= 214)
        exportGvarParam(mix.sOffset, _offset, version);
      else
        splitGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
    }

    virtual void afterImport()
    {
      if (mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix.destCh = _destCh + 1;
        if (!IS_STM32(board) && version < 218) {
          if (!_curveMode)
            mix.curve = CurveReference(CurveReference::CURVE_REF_DIFF, smallGvarExport(_curveParam));
          else if (_curveParam > 6)
            mix.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam-6);
          else if (_curveParam < 0)
            mix.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam);
          else
            mix.curve = CurveReference(CurveReference::CURVE_REF_FUNC, _curveParam);
        }
      }

      importGvarParam(mix.weight, _weight, version);
      importGvarParam(mix.sOffset, _offset, version);

      qCDebug(eepromImport) << QString("imported %1: ch %2, name '%3'").arg(internalField.getName()).arg(mix.destCh).arg(mix.name);
    }

  protected:
    StructField internalField;
    MixData & mix;
    Board::Type board;
    unsigned int version;
    ModelData * model;
    unsigned int _destCh;
    bool _curveMode;
    int _curveParam;
    int _weight;
    int _offset;
    unsigned int _weightMode;
    unsigned int _offsetMode;
};

class InputField: public TransformedField {
  public:
    InputField(DataField * parent, ExpoData & expo, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Input"),
      expo(expo),
      board(board),
      version(version)
    {
      if (version >= 218) {
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<14>(this, expo.scale, "Scale"));
        internalField.Append(new SourceField<10>(this, expo.srcRaw, board, version, 0));
        internalField.Append(new SignedField<6>(this, expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<5>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<9>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<9>(this, expo.flightModes));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new SpareBitsField<1>(this));
        if (IS_HORUS(board))
          internalField.Append(new ZCharField<6>(this, expo.name));
        else if (HAS_LARGE_LCD(board) && version <= 218)
          internalField.Append(new ZCharField<8>(this, expo.name));
        else
          internalField.Append(new ZCharField<6>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _offset, "Offset"));
        internalField.Append(new CurveReferenceField(this, expo.curve, board, version));
      }
      else if (IS_TARANIS(board)) {
        internalField.Append(new SourceField<10>(this, expo.srcRaw, board, version, 0));
        internalField.Append(new UnsignedField<14>(this, expo.scale, "Scale"));
        internalField.Append(new UnsignedField<8>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(this, expo.flightModes));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new SignedField<6>(this, expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new ZCharField<8>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _offset, "Offset"));
        internalField.Append(new CurveReferenceField(this, expo.curve, board, version));
      }
      else {
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<3>(this, expo.chn, "Channel"));
        internalField.Append(new BoolField<2>(this, _curveMode));
        internalField.Append(new UnsignedField<9>(this, expo.flightModes, "Phases"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new ZCharField<6>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
    }

    virtual void beforeExport()
    {
      _weight = smallGvarImport(expo.weight);

      if (IS_HORUS_OR_TARANIS(board) || version >= 218) {
        _offset = smallGvarImport(expo.offset);
      }

      if (!IS_TARANIS(board) && version < 218) {
        if (expo.curve.type==CurveReference::CURVE_REF_FUNC && expo.curve.value) {
          _curveMode = true;
          _curveParam = expo.curve.value;
        }
        else if (expo.curve.type==CurveReference::CURVE_REF_CUSTOM && expo.curve.value) {
          _curveMode = true;
          _curveParam = expo.curve.value+6;
        }
        else {
          _curveMode = false;
          _curveParam = smallGvarImport(expo.curve.value);
        }
      }
    }

    virtual void afterImport()
    {
      if (!IS_STM32(board) && expo.mode) {
        expo.srcRaw = RawSource(SOURCE_TYPE_STICK, expo.chn);
      }

      expo.weight = smallGvarExport(_weight);

      if (IS_STM32(board) || version >= 218) {
        expo.offset = smallGvarExport(_offset);
      }

      if (!IS_TARANIS(board) && version < 218) {
        if (!_curveMode)
          expo.curve = CurveReference(CurveReference::CURVE_REF_EXPO, smallGvarExport(_curveParam));
        else if (_curveParam > 6)
          expo.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam-6);
        else
          expo.curve = CurveReference(CurveReference::CURVE_REF_FUNC, _curveParam);
      }
      qCDebug(eepromImport) << QString("imported %1: ch %2 name '%3'").arg(internalField.getName()).arg(expo.chn).arg(expo.name);
    }

  protected:
    StructField internalField;
    ExpoData & expo;
    Board::Type board;
    unsigned int version;
    bool _curveMode;
    int  _weight;
    int  _offset;
    int  _curveParam;
};

class LimitField: public StructField {
  public:
    template <int shift, int GV1>
    static int exportLimitValue(int value)
    {
      if (value > 10000) {
        return -GV1 + value - 10001;
      }
      if (value < -10000) {
        return GV1 + value + 10000;
      }
      return value + shift;
    }

    template <int shift, int GV1>
    static int importLimitValue(int value)
    {
      if (value >= GV1) {
        return 10001 + value - GV1;
      }
      if (value >= GV1-9) {
        return -10000 + value - GV1;
      }
      if (value < -GV1) {
        return -10000 + value + GV1 + 1;
      }
      if (value < -GV1+9) {
        return 10000 + value + GV1 + 1;
      }
      return value - shift;
    }

    LimitField(DataField * parent, LimitData & limit, Board::Type board, unsigned int version):
      StructField(parent, "Limit")
    {
      if (version >= 218) {
        Append(new ConversionField< SignedField<11> >(this, limit.min, exportLimitValue<1000, 1024>, importLimitValue<1000, 1024>));
        Append(new ConversionField< SignedField<11> >(this, limit.max, exportLimitValue<-1000, 1024>, importLimitValue<-1000, 1024>));
        Append(new SignedField<10>(this, limit.ppmCenter));
        Append(new ConversionField< SignedField<11> >(this, limit.offset, exportLimitValue<0, 1024>, importLimitValue<0, 1024>));
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        Append(new SpareBitsField<3>(this));
        Append(new SignedField<8>(this, limit.curve.value));
        if (HAS_LARGE_LCD(board))
          Append(new ZCharField<6>(this, limit.name));
        else
          Append(new ZCharField<4>(this, limit.name));
      }
      else if (IS_TARANIS(board)) {
        Append(new ConversionField< SignedField<11> >(this, limit.min, exportLimitValue<1000, 1024>, importLimitValue<1000, 1024>));
        Append(new ConversionField< SignedField<11> >(this, limit.max, exportLimitValue<-1000, 1024>, importLimitValue<-1000, 1024>));
        Append(new SignedField<10>(this, limit.ppmCenter));
        Append(new ConversionField< SignedField<11> >(this, limit.offset, exportLimitValue<0, 1024>, importLimitValue<0, 1024>));
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        Append(new SpareBitsField<3>(this));
        Append(new SignedField<8>(this, limit.curve.value));
        Append(new ZCharField<6>(this, limit.name));
      }
      else {
        Append(new ConversionField< SignedField<8> >(this, limit.min, +100, 10));
        Append(new ConversionField< SignedField<8> >(this, limit.max, -100, 10));
        Append(new SignedField<8>(this, limit.ppmCenter));
        Append(new SignedField<14>(this, limit.offset));
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        if (HAS_LARGE_LCD(board)) {
          Append(new ZCharField<6>(this, limit.name));
        }
      }
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(DataField * parent, CurveData * curves, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Curves"),
      curves(curves),
      board(board),
      version(version),
      maxCurves(MAX_CURVES(board, version)),
      maxPoints(512)
    {
      for (int i=0; i<maxCurves; i++) {
        if (version >= 218) {
          internalField.Append(new UnsignedField<1>(this, (unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(this, curves[i].smooth));
          internalField.Append(new ConversionField< SignedField<6> >(this, curves[i].count, -5));
          internalField.Append(new ZCharField<3>(this, curves[i].name));
        }
        else if (IS_TARANIS(board)) {
          internalField.Append(new UnsignedField<3>(this, (unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(this, curves[i].smooth));
          internalField.Append(new SpareBitsField<4>(this));
          internalField.Append(new ConversionField< SignedField<8> >(this, curves[i].count, -5));
        }
        else {
          internalField.Append(new SignedField<16>(this, _curves[i]));
        }
      }

      for (int i=0; i<maxPoints; i++) {
        internalField.Append(new SignedField<8>(this, _points[i]));
      }
    }

    virtual void beforeExport()
    {
      memset(_points, 0, sizeof(_points));

      int * cur = &_points[0];
      int offset = 0;

      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        if (IS_STM32(board)) {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count);
          if (offset > maxPoints) {
            setError(DataField::tr("OpenTX only accepts %1 points in all curves").arg(maxPoints));
            break;
          }
        }
        else {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count) - 5;
          if (offset > maxPoints - 5 * maxCurves) {
            setError(DataField::tr("OpenTx only accepts %1 points in all curves").arg(maxPoints));
            break;
          }
          _curves[i] = offset;
        }
        for (int j=0; j<curve->count; j++) {
          *cur++ = curve->points[j].y;
        }
        if (curve->type == CurveData::CURVE_TYPE_CUSTOM) {
          for (int j=1; j<curve->count-1; j++) {
            *cur++ = curve->points[j].x;
          }
        }
      }
    }

    virtual void afterImport()
    {
      int * cur = &_points[0];

      for (int i=0; i<maxCurves; i++) {
        CurveData * curve = &curves[i];
        if (!IS_STM32(board) && version < 218) {
          int * next = &_points[5*(i+1) + _curves[i]];
          int size = next - cur;
          if (size % 2 == 0) {
            curve->count = (size / 2) + 1;
            curve->type = CurveData::CURVE_TYPE_CUSTOM;
          }
          else {
            curve->count = size;
            curve->type = CurveData::CURVE_TYPE_STANDARD;
          }
        }

        if (curve->count > 17) {
          qDebug() << "Wrong points count" << curve->count;
          curve->count = 0;
          for (int j=0; j<maxCurves; j++) {
            CurveData *curve = &curves[j];
            curve->clear(5);
          }
          return;
        }

        for (int j=0; j<curve->count; j++) {
          curve->points[j].y = *cur++;
        }

        if (curve->type == CurveData::CURVE_TYPE_CUSTOM) {
          curve->points[0].x = -100;
          for (int j=1; j<curve->count-1; j++)
            curve->points[j].x = *cur++;
          curve->points[curve->count-1].x = +100;
        }
        else {
          for (int j=0; j<curve->count; j++)
            curve->points[j].x = -100 + (200*i) / (curve->count-1);
        }
        qCDebug(eepromImport) << QString("imported curve: %3 points").arg(curve->count);
      }
    }

  protected:
    StructField internalField;
    CurveData * curves;
    Board::Type board;
    unsigned int version;
    int maxCurves;
    int maxPoints;
    int _curves[CPN_MAX_CURVES];
    int _points[CPN_MAX_CURVES*CPN_MAX_POINTS*2];
};

class LogicalSwitchesFunctionsTable: public ConversionTable {

  public:
    LogicalSwitchesFunctionsTable(Board::Type board, unsigned int version)
    {
      int val=0;
      addConversion(LS_FN_OFF, val++);
      addConversion(LS_FN_VEQUAL, val++);
      addConversion(LS_FN_VALMOSTEQUAL, val++);
      addConversion(LS_FN_VPOS, val++);
      addConversion(LS_FN_VNEG, val++);
      val++; // later RANGE
      addConversion(LS_FN_APOS, val++);
      addConversion(LS_FN_ANEG, val++);
      addConversion(LS_FN_AND, val++);
      addConversion(LS_FN_OR, val++);
      addConversion(LS_FN_XOR, val++);
      addConversion(LS_FN_EDGE, val++);
      addConversion(LS_FN_EQUAL, val++);
      addConversion(LS_FN_GREATER, val++);
      addConversion(LS_FN_LESS, val++);
      addConversion(LS_FN_DPOS, val++);
      addConversion(LS_FN_DAPOS, val++);
      addConversion(LS_FN_TIMER, val++);
      addConversion(LS_FN_STICKY, val++);
    }
};

class AndSwitchesConversionTable: public ConversionTable {

  public:
    AndSwitchesConversionTable(Board::Type board, unsigned int version)
    {
      int val=0;
      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      if (IS_STM32(board)) {
        for (int i=1; i<=MAX_SWITCHES_POSITION(board, version); i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
      else if (IS_SKY9X(board)) {
        for (int i=1; i<=8; i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
      else {
        for (int i=1; i<=9; i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=7; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
    }

    static ConversionTable * getInstance(Board::Type board, unsigned int version)
    {
      return new SwitchesConversionTable(board, version);
    }


  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }
};

class LogicalSwitchField: public TransformedField {
  public:
    LogicalSwitchField(DataField * parent, LogicalSwitchData & csw, Board::Type board, unsigned int version, unsigned int variant, ModelData * model=nullptr):
      TransformedField(parent, internalField),
      internalField(this, "LogicalSwitch"),
      csw(csw),
      board(board),
      version(version),
      variant(variant),
      model(model),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, 0)),
      switchesConversionTable(SwitchesConversionTable::getInstance(board, version)),
      andswitchesConversionTable(AndSwitchesConversionTable::getInstance(board, version)),
      v1(0),
      v2(0),
      v3(0)
    {
      if (version >= 218) {
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(this, v1));
        internalField.Append(new SignedField<10>(this, v3));
        internalField.Append(new ConversionField< SignedField<9> >(this, (int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        internalField.Append(new SpareBitsField<1>(this)); // TODO extra switch mode
        internalField.Append(new SpareBitsField<2>(this));
        internalField.Append(new SignedField<16>(this, v2));
      }
      else {
        internalField.Append(new ConversionField< UnsignedField<6> >(this, csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(this, v1));
        internalField.Append(new SignedField<16>(this, v2));
        internalField.Append(new SignedField<16>(this, v3));
      }

      int scale = 0;
      internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.delay, 0, scale));
      internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.duration, 0, scale));
      if (version == 217) {
        internalField.Append(new ConversionField< SignedField<8> >(this, (int &)csw.andsw, andswitchesConversionTable, "AND switch"));
      }
    }

    ~LogicalSwitchField()
    {
      delete andswitchesConversionTable;
    }

    virtual void beforeExport()
    {
      if (csw.func == LS_FN_TIMER) {
        v1 = csw.val1;
        v2 = csw.val2;
      }
      else if (csw.func == LS_FN_EDGE) {
        switchesConversionTable->exportValue(csw.val1, v1);
        v2 = csw.val2;
        v3 = csw.val3;
      }
      else if ((csw.func >= LS_FN_AND && csw.func <= LS_FN_XOR) || csw.func == LS_FN_STICKY) {
        switchesConversionTable->exportValue(csw.val1, v1);
        switchesConversionTable->exportValue(csw.val2, v2);
      }
      else if (csw.func >= LS_FN_EQUAL && csw.func <= LS_FN_ELESS) {
        sourcesConversionTable->exportValue(csw.val1, v1);
        sourcesConversionTable->exportValue(csw.val2, v2);
      }
      else if (csw.func != LS_FN_OFF) {
        sourcesConversionTable->exportValue(csw.val1, v1);
        v2 = csw.val2;
      }
    }

    virtual void afterImport()
    {
      if (csw.func == LS_FN_TIMER) {
        csw.val1 = v1;
        csw.val2 = v2;
      }
      else if (csw.func == LS_FN_EDGE) {
        switchesConversionTable->importValue(v1, csw.val1);
        csw.val2 = v2;
        csw.val3 = v3;
      }
      else if ((csw.func >= LS_FN_AND && csw.func <= LS_FN_XOR) || csw.func == LS_FN_STICKY) {
        switchesConversionTable->importValue(v1, csw.val1);
        switchesConversionTable->importValue(v2, csw.val2);
      }
      else if (csw.func >= LS_FN_EQUAL && csw.func <= LS_FN_ELESS) {
        sourcesConversionTable->importValue((uint32_t)v1, csw.val1);
        sourcesConversionTable->importValue((uint32_t)v2, csw.val2);
      }
      else if (csw.func != LS_FN_OFF) {
        sourcesConversionTable->importValue((uint32_t)v1, csw.val1);
        csw.val2 = v2;
        RawSource val1(csw.val1);
      }
      qCDebug(eepromImport) << QString("imported %1: %2").arg(internalField.getName()).arg(csw.funcToString());
    }

  protected:
    StructField internalField;
    LogicalSwitchData & csw;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    ModelData * model;
    LogicalSwitchesFunctionsTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    SwitchesConversionTable * switchesConversionTable;
    ConversionTable * andswitchesConversionTable;
    int v1;
    int v2;
    int v3;
};

class CustomFunctionsConversionTable: public ConversionTable {

  public:
    CustomFunctionsConversionTable(Board::Type board, unsigned int version)
    {
      int val=0;

      for (int i=0; i<MAX_CHANNELS(board, version); i++) {
        addConversion(i, val);
      }
      val++;

      addConversion(FuncTrainer, val);
      addConversion(FuncTrainerRUD, val);
      addConversion(FuncTrainerELE, val);
      addConversion(FuncTrainerTHR, val);
      addConversion(FuncTrainerAIL, val);
      val++;

      addConversion(FuncInstantTrim, val++);

      addConversion(FuncReset, val++);
      addConversion(FuncSetTimer1, val);
      addConversion(FuncSetTimer2, val);
      addConversion(FuncSetTimer3, val);
      val++;
      for (int i=0; i<MAX_GVARS(board, version); i++)
        addConversion(FuncAdjustGV1+i, val);
      val++;
      addConversion(FuncVolume, val++);
      addConversion(FuncSetFailsafeInternalModule, val);
      addConversion(FuncSetFailsafeExternalModule, val++);
      addConversion(FuncRangeCheckInternalModule, val);
      addConversion(FuncRangeCheckExternalModule, val++);
      addConversion(FuncBindInternalModule, val);
      addConversion(FuncBindExternalModule, val++);
      addConversion(FuncPlaySound, val++);
      addConversion(FuncPlayPrompt, val++);
      addConversion(FuncPlayValue, val++);
      addConversion(FuncReserve, val++);
      if (IS_STM32(board))
        addConversion(FuncPlayScript, val++);
      else
        addConversion(FuncReserve, val++);
      addConversion(FuncReserve, val++);
      addConversion(FuncBackgroundMusic, val++);
      addConversion(FuncBackgroundMusicPause, val++);
      addConversion(FuncVario, val++);
      addConversion(FuncPlayHaptic, val++);
      addConversion(FuncLogs, val++);
      addConversion(FuncBacklight, val++);
      if (IS_STM32(board))
        addConversion(FuncScreenshot, val++);
    }
};

template <int N>
class SwitchesWarningField: public TransformedField {
  public:
    SwitchesWarningField(DataField * parent, uint64_t & sw, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, _sw, "SwitchesWarning"),
      sw(sw),
      board(board),
      version(version)
    {
    }

    virtual void beforeExport()
    {
      _sw = sw;
    }

    virtual void afterImport()
    {
      sw = _sw;
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    BaseUnsignedField<uint64_t, N> internalField;
    uint64_t & sw;
    uint64_t _sw;
    Board::Type board;
    unsigned int version;
};

class ArmCustomFunctionField: public TransformedField {
  public:
    ArmCustomFunctionField(DataField * parent, CustomFunctionData & fn, Board::Type board, unsigned int version, unsigned int variant):
      TransformedField(parent, internalField),
      internalField(this, "CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, 0)),
      _func(0),
      _active(0),
      _mode(0)
    {
      memset(_param, 0, sizeof(_param));

      if (version >= 218) {
        internalField.Append(new SwitchField<9>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<7> >(this, _func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      }
      else {
        internalField.Append(new SwitchField<8>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, _func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      }

      if (IS_TARANIS(board))
        internalField.Append(new CharField<8>(this, _param, false));
      else
        internalField.Append(new CharField<6>(this, _param, false));

      internalField.Append(new SignedField<8>(this, _active));
    }

    static bool hasRepeatParam(const CustomFunctionData & fn)
    {
      return (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue || fn.func == FuncPlayHaptic);
    }

    virtual void beforeExport()
    {
      if (fn.swtch.type != SWITCH_TYPE_NONE) {
        _func = fn.func;

        if (hasRepeatParam(fn))
          _active = fn.repeatParam;
        else
          _active = (fn.enabled ? 1 : 0);

        if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
          *((uint16_t *)_param) = fn.param;
          *((uint8_t *)(_param+3)) = fn.func - FuncOverrideCH1;
        }
        else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
          *((uint8_t *)(_param+3)) = fn.func - FuncTrainer;
        }
        else if (fn.func >= FuncSetTimer1 && fn.func <= FuncSetTimer3) {
          *((uint16_t *)_param) = fn.param;
          *((uint8_t *)(_param+3)) = fn.func - FuncSetTimer1;
        }
        else if (fn.func >= FuncSetFailsafeInternalModule && fn.func <= FuncSetFailsafeExternalModule) {
          *((uint16_t *)_param) = fn.func - FuncSetFailsafeInternalModule;
        }
        else if (fn.func >= FuncRangeCheckInternalModule && fn.func <= FuncRangeCheckExternalModule) {
          *((uint16_t *)_param) = fn.func - FuncRangeCheckInternalModule;
        }
        else if (fn.func >= FuncBindInternalModule && fn.func <= FuncBindExternalModule) {
          *((uint16_t *)_param) = fn.func - FuncBindInternalModule;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic || fn.func == FuncPlayScript) {
          memcpy(_param, fn.paramarm, sizeof(_param));
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
          *((uint8_t *)(_param+2)) = fn.adjustMode;
          *((uint8_t *)(_param+3)) = fn.func - FuncAdjustGV1;
          unsigned int value;
          if (fn.adjustMode == 1)
            sourcesConversionTable->exportValue(fn.param, (int &)value);
          else if (fn.adjustMode == 2)
            value = RawSource(fn.param).index;
          else
            value = fn.param;
          *((uint16_t *)_param) = value;
        }
        else if (fn.func == FuncPlayValue || fn.func == FuncVolume) {
          unsigned int value;
          sourcesConversionTable->exportValue(fn.param, (int &)value);
          *((uint16_t *)_param) = value;
        }
        else if (fn.func == FuncReset) {
          *((uint32_t *)_param) = fn.param;
        }
        else {
          *((uint32_t *)_param) = fn.param;
        }
      }
    }

    virtual void afterImport()
    {
      fn.func = (AssignFunc)_func;

      if (hasRepeatParam(fn))
        fn.repeatParam = _active;
      else
        fn.enabled = (_active & 0x01);

      unsigned int value=0, mode=0, index=0;
      value = *((uint16_t *)_param);
      mode = *((uint8_t *)(_param+2));
      index = *((uint8_t *)(_param+3));

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = (int16_t)(uint16_t)value;
      }
      else if (fn.func >= FuncSetTimer1 && fn.func <= FuncSetTimer3) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = (int)value;
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = value;
      }
      else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic || fn.func == FuncPlayScript) {
        memcpy(fn.paramarm, _param, sizeof(fn.paramarm));
      }
      else if (fn.func == FuncVolume) {
        sourcesConversionTable->importValue(value, (int &)fn.param);
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        fn.func = AssignFunc(fn.func + index);
        fn.adjustMode = mode;
        if (fn.adjustMode == 1)
          sourcesConversionTable->importValue(value, (int &)fn.param);
        else if (fn.adjustMode == 2)
          fn.param = RawSource(SOURCE_TYPE_GVAR, value).toValue();
        else
          fn.param = (int16_t)value;
      }
      else if (fn.func == FuncPlayValue) {
        sourcesConversionTable->importValue(value, (int &)fn.param);
      }
      else if (fn.func == FuncReset) {
        fn.param = value;
      }
      else {
        fn.param = value;
      }
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    unsigned int _func;
    char _param[10];
    int _active;
    unsigned int _mode;
};

class AvrCustomFunctionField: public TransformedField {
  public:
    AvrCustomFunctionField(DataField * parent, CustomFunctionData & fn, Board::Type board, unsigned int version, unsigned int variant):
      TransformedField(parent, internalField),
      internalField(this, "CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, 0)),
      _param(0),
      _mode(0),
      _union_param(0),
      _active(0)
    {
      internalField.Append(new SwitchField<6>(this, fn.swtch, board, version));
      internalField.Append(new ConversionField< UnsignedField<4> >(this, (unsigned int &)fn.func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      internalField.Append(new UnsignedField<5>(this, _union_param));
      internalField.Append(new UnsignedField<1>(this, _active));
      internalField.Append(new UnsignedField<8>(this, _param));
    }

    virtual void beforeExport()
    {
      _param = fn.param;
      _active = (fn.enabled ? 1 : 0);

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        _union_param = fn.func - FuncOverrideCH1;
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        _union_param = fn.func - FuncTrainer;
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        _union_param = fn.adjustMode;
        _union_param += (fn.func - FuncAdjustGV1) << 2;
        if (fn.adjustMode == 1)
          sourcesConversionTable->exportValue(fn.param, (int &)_param);
        else if (fn.adjustMode == 2)
          _param = RawSource(fn.param).index;
      }
      else if (fn.func == FuncPlayValue) {
        _union_param = fn.repeatParam / 10;
        sourcesConversionTable->exportValue(fn.param, (int &)_param);
      }
      else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
        _union_param = fn.repeatParam / 10;
      }
    }

    virtual void afterImport()
    {
      fn.param = _param;
      fn.enabled = (_active & 0x01);

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        fn.func = AssignFunc(fn.func + _union_param);
        fn.param = (int8_t)fn.param;
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        fn.func = AssignFunc(fn.func + _union_param);
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        fn.func = AssignFunc(fn.func + (_union_param >> 2));
        fn.adjustMode = (_union_param & 0x03);
        if (fn.adjustMode == 1)
          sourcesConversionTable->importValue(_param, (int &)fn.param);
        else if (fn.adjustMode == 2)
          fn.param = RawSource(SOURCE_TYPE_GVAR, _param).toValue();
      }
      else if (fn.func == FuncPlayValue) {
        fn.repeatParam = _union_param * 10;
        sourcesConversionTable->importValue(_param, (int &)fn.param);
      }
      else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
        fn.repeatParam = _union_param * 10;
      }
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    unsigned int _param;
    unsigned int _mode;
    unsigned int _union_param;
    unsigned int _active;
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(DataField * parent, FrSkyScreenData & screen, Board::Type board, unsigned int version, unsigned int variant):
      DataField(parent, "Frsky Screen"),
      screen(screen),
      board(board),
      version(version),
      none(this),
      bars(this),
      numbers(this),
      script(this)
    {
      for (int i=0; i<4; i++) {
        if (IS_TARANIS(board))
          bars.Append(new SourceField<16>(this, screen.body.bars[i].source, board, version, variant));
        else
          bars.Append(new SourceField<8>(this, screen.body.bars[i].source, board, version, variant));
        bars.Append(new SignedField<16>(this, screen.body.bars[i].barMin));
        bars.Append(new SignedField<16>(this, screen.body.bars[i].barMax));
      }

      int columns = (HAS_LARGE_LCD(board) ? 3 : 2);
      for (int i=0; i<4; i++) {
        for (int j=0; j<columns; j++) {
          if (IS_TARANIS(board))
            numbers.Append(new SourceField<16>(this, screen.body.lines[i].source[j], board, version, variant));
          else
            numbers.Append(new SourceField<8>(this, screen.body.lines[i].source[j], board, version, variant));
        }
      }
      for (unsigned i=numbers.size(); i<bars.size(); i++) {
        numbers.Append(new SpareBitsField<1>(this));
      }

      if (IS_TARANIS(board)) {
        script.Append(new CharField<8>(this, screen.body.script.filename, true, "Script name"));
        script.Append(new SpareBitsField<16*8>(this));
      }

      if (IS_TARANIS(board))
        none.Append(new SpareBitsField<24*8>(this));
      else
        none.Append(new SpareBitsField<20*8>(this));
    }

    virtual void ExportBits(QBitArray & output)
    {
      if (screen.type == TELEMETRY_SCREEN_SCRIPT)
        script.ExportBits(output);
      else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
        numbers.ExportBits(output);
      else if (screen.type == TELEMETRY_SCREEN_BARS)
        bars.ExportBits(output);
      else
        none.ExportBits(output);
    }

    virtual void ImportBits(const QBitArray & input)
    {
      qCDebug(eepromImport) << QString("importing %1: type: %2").arg(name).arg(screen.type);

      // NOTA: screen.type should have been imported first!
      if (screen.type == TELEMETRY_SCREEN_SCRIPT)
        script.ImportBits(input);
      else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
        numbers.ImportBits(input);
      else if (screen.type == TELEMETRY_SCREEN_BARS)
        bars.ImportBits(input);
      else
        none.ImportBits(input);
    }

    virtual unsigned int size()
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == TELEMETRY_SCREEN_SCRIPT)
        return script.size();
      else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
        return numbers.size();
      else if (screen.type == TELEMETRY_SCREEN_BARS)
        return bars.size();
      else
        return none.size();
    }

  protected:
    FrSkyScreenData & screen;
    Board::Type board;
    unsigned int version;
    StructField none;
    StructField bars;
    StructField numbers;
    StructField script;
};

class RSSIConversionTable: public ConversionTable
{
  public:
    RSSIConversionTable(int index)
    {
      addConversion(0, 2-index);
      addConversion(1, 3-index);
      addConversion(2, index ? 3 : 0);
      addConversion(3, 1-index);
    }

    RSSIConversionTable()
    {
    }
};

class TelemetryVarioSourceConversionTable: public ConversionTable
{
  public:
    TelemetryVarioSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      if (!IS_HORUS_OR_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI, val++);
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI_PLUS, val++);
      }
      addConversion(TELEMETRY_VARIO_SOURCE_VSPEED, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A1, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A2, val++);
      if (IS_HORUS_OR_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_DTE, val++);
      }
    }
};

class TelemetryVoltsSourceConversionTable: public ConversionTable
{
  public:
    TelemetryVoltsSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      addConversion(TELEMETRY_VOLTS_SOURCE_A1, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_A2, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_A3, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_A4, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_FAS, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_CELLS, val++);
    }
};

class ScreenTypesConversionTable: public ConversionTable
{
  public:
    ScreenTypesConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      addConversion(TELEMETRY_SCREEN_NONE, val++);
      addConversion(TELEMETRY_SCREEN_NUMBERS, val++);
      addConversion(TELEMETRY_SCREEN_BARS, val++);
    }
};

class TelemetryCurrentSourceConversionTable: public ConversionTable
{
  public:
    TelemetryCurrentSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      addConversion(TELEMETRY_CURRENT_SOURCE_NONE, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A1, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A2, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A3, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A4, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_FAS, val++);
    }
};

class FrskyField: public StructField {
  public:
    FrskyField(DataField * parent, FrSkyData & frsky, RSSIAlarmData & rssiAlarms, Board::Type board, unsigned int version, unsigned int variant):
      StructField(parent, "FrSky"),
      telemetryVarioSourceConversionTable(board, version),
      screenTypesConversionTable(board, version),
      telemetryVoltsSourceConversionTable(board, version),
      telemetryCurrentSourceConversionTable(board, version)
    {
      if (version <= 218) {
        rssiConversionTable[0] = RSSIConversionTable(0);
        rssiConversionTable[1] = RSSIConversionTable(1);

        if (!IS_HORUS(board)) {
          Append(new UnsignedField<8>(this, frsky.voltsSource, "Volts Source"));
          Append(new UnsignedField<8>(this, frsky.altitudeSource, "Altitude Source"));

          for (int i = 0; i < 4; i++) {
            Append(new UnsignedField<2>(this, frsky.screens[i].type));
          }
          for (int i = 0; i < 4; i++) {
            Append(new FrskyScreenField(this, frsky.screens[i], board, version, variant));
          }
        }
      }

      Append(new UnsignedField<7>(this, frsky.varioSource, "Vario Source"));
      Append(new BoolField<1>(this, frsky.varioCenterSilent));
      Append(new SignedField<8>(this, frsky.varioCenterMax));
      Append(new SignedField<8>(this, frsky.varioCenterMin));
      Append(new SignedField<8>(this, frsky.varioMin));
      Append(new SignedField<8>(this, frsky.varioMax));
      if (version <= 218) {
        Append(new BoolField<1>(this, rssiAlarms.disabled));
        Append(new SpareBitsField<1>(this));
        Append(new ConversionField<SignedField<6> >(this, rssiAlarms.warning, -45));
        Append(new SpareBitsField<2>(this));
        Append(new ConversionField<SignedField<6> >(this, rssiAlarms.critical, -42));
      }
    }

  protected:
    RSSIConversionTable rssiConversionTable[2];
    TelemetryVarioSourceConversionTable telemetryVarioSourceConversionTable;
    ScreenTypesConversionTable screenTypesConversionTable;
    TelemetryVoltsSourceConversionTable telemetryVoltsSourceConversionTable;
    TelemetryCurrentSourceConversionTable telemetryCurrentSourceConversionTable;
};

/*
 * TODO
 */
#if 0
class CustomScreenField: public StructField {
  public:
    CustomScreenField(CustomScreenData & customScreen):
      StructField("Custom Screen"),
      customScreen(customScreen)
    {
    }

  protected:
    CustomScreenData & customScreen;
};
#endif

class SensorField: public TransformedField {
  public:
    SensorField(DataField * parent, SensorData & sensor, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Sensor"),
      sensor(sensor),
      version(version),
      _param(0)
    {
      internalField.Append(new UnsignedField<16>(this, _id, "id/persistentValue"));
      internalField.Append(new UnsignedField<8>(this, _instance, "instance/formula"));
      internalField.Append(new ZCharField<4>(this, sensor.label));
      if (version >= 219) {
        internalField.Append(new UnsignedField<8>(this, _subid, "subid"));
        internalField.Append(new UnsignedField<2>(this, sensor.type, "type"));
        internalField.Append(new UnsignedField<6>(this, sensor.unit, "unit"));
        internalField.Append(new UnsignedField<2>(this, sensor.prec, "prec"));
        internalField.Append(new BoolField<1>(this, sensor.autoOffset));
        internalField.Append(new BoolField<1>(this, sensor.filter));
        internalField.Append(new BoolField<1>(this, sensor.logs));
        internalField.Append(new BoolField<1>(this, sensor.persistent));
        internalField.Append(new BoolField<1>(this, sensor.onlyPositive));
        internalField.Append(new SpareBitsField<1>(this));
      }
      else {
        internalField.Append(new UnsignedField<1>(this, sensor.type, "type"));
        internalField.Append(new UnsignedField<5>(this, sensor.unit, "unit"));
        internalField.Append(new UnsignedField<2>(this, sensor.prec, "prec"));
        internalField.Append(new BoolField<1>(this, sensor.autoOffset));
        internalField.Append(new BoolField<1>(this, sensor.filter));
        internalField.Append(new BoolField<1>(this, sensor.logs));
        internalField.Append(new BoolField<1>(this, sensor.persistent));
        internalField.Append(new BoolField<1>(this, sensor.onlyPositive));
        internalField.Append(new UnsignedField<3>(this, _subid, "subid"));
      }
      internalField.Append(new UnsignedField<32>(this, _param, "param"));
    }

    virtual void beforeExport()
    {
      if (sensor.type == SensorData::TELEM_TYPE_CUSTOM) {
        _id = sensor.id;
        _subid = sensor.subid;
        _instance = sensor.instance;
        _ratio = sensor.ratio;
        _offset = sensor.offset;
      }
      else {
        _id = sensor.persistentValue;
        _instance = sensor.formula;
        if (sensor.formula == SensorData::TELEM_FORMULA_CELL)
          _param = (sensor.source) + (sensor.index << 8);
        else if (sensor.formula == SensorData::TELEM_FORMULA_ADD || sensor.formula == SensorData::TELEM_FORMULA_AVERAGE || sensor.formula == SensorData::TELEM_FORMULA_MULTIPLY || sensor.formula == SensorData::TELEM_FORMULA_MIN || sensor.formula == SensorData::TELEM_FORMULA_MAX)
          _param = ((uint8_t)sensor.sources[0]) + ((uint8_t)sensor.sources[1] << 8) + ((uint8_t)sensor.sources[2] << 16) + ((uint8_t)sensor.sources[3] << 24);
        else if (sensor.formula == SensorData::TELEM_FORMULA_DIST)
          _param = (sensor.gps) + (sensor.alt << 8);
        else if (sensor.formula == SensorData::TELEM_FORMULA_CONSUMPTION || sensor.formula == SensorData::TELEM_FORMULA_TOTALIZE)
          _param = (sensor.amps);
      }
    }

    virtual void afterImport()
    {
      if (sensor.type == SensorData::TELEM_TYPE_CUSTOM) {
        sensor.id = _id;
        sensor.subid = _subid;
        sensor.instance = _instance;
        sensor.ratio = _ratio;
        sensor.offset = _offset;
      }
      else {
        sensor.persistentValue = _id;
        sensor.formula = _instance;
        if (sensor.formula == SensorData::TELEM_FORMULA_CELL)
          (sensor.source = _sources[0], sensor.index = _sources[1]);
        else if (sensor.formula == SensorData::TELEM_FORMULA_ADD || sensor.formula == SensorData::TELEM_FORMULA_AVERAGE || sensor.formula == SensorData::TELEM_FORMULA_MULTIPLY || sensor.formula == SensorData::TELEM_FORMULA_MIN || sensor.formula == SensorData::TELEM_FORMULA_MAX)
          for (int i=0; i<4; ++i)
            sensor.sources[i] = _sources[i];
        else if (sensor.formula == SensorData::TELEM_FORMULA_DIST)
          (sensor.gps = _sources[0], sensor.alt = _sources[1]);
        else if (sensor.formula == SensorData::TELEM_FORMULA_CONSUMPTION || sensor.formula == SensorData::TELEM_FORMULA_TOTALIZE)
          sensor.amps = _sources[0];
      }

      if (version < 218) {
        if (sensor.unit > SensorData::UNIT_WATTS)
          sensor.unit++;
        if (sensor.unit > SensorData::UNIT_DEGREE)
          sensor.unit++;
      }

      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    SensorData & sensor;
    unsigned int version;
    unsigned int _id;
    unsigned int _subid;
    unsigned int _instance;
    union {
      unsigned int _param;
      uint8_t _sources[4];
      struct {
        uint16_t _ratio;
        int16_t _offset;
      };
    };
};

int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

OpenTxModelData::OpenTxModelData(ModelData & modelData, Board::Type board, unsigned int version, unsigned int variant):
  TransformedField(nullptr, internalField),
  internalField(this, "ModelData"),
  modelData(modelData),
  board(board),
  version(version),
  variant(variant),
  protocolsConversionTable(board)
{
  sprintf(name, "Model %s", modelData.name);

  qDebug() << QString("OpenTxModelData::OpenTxModelData(name: %1, board: %2, ver: %3, var: %4)").arg(name).arg(board).arg(version).arg(variant);

  if (IS_HORUS(board))
    internalField.Append(new ZCharField<15>(this, modelData.name, "Model name"));
  else if (HAS_LARGE_LCD(board))
    internalField.Append(new ZCharField<12>(this, modelData.name, "Model name"));
  else
    internalField.Append(new ZCharField<10>(this, modelData.name, "Model name"));

  internalField.Append(new UnsignedField<8>(this, modelData.moduleData[0].modelId));
  internalField.Append(new UnsignedField<8>(this, modelData.moduleData[1].modelId));

  if (HAS_LARGE_LCD(board)) {
    internalField.Append(new CharField<10>(this, modelData.bitmap, true, "Model bitmap"));
  }

  for (int i=0; i<MAX_TIMERS(board, version); i++) {
    if (version >= 218) {
      internalField.Append(new SwitchField<9>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<23>(this, modelData.timers[i].val));
      internalField.Append(new SignedField<24>(this, modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>(this));
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<8>(this, modelData.timers[i].name, "Timer name"));
      else
        internalField.Append(new ZCharField<3>(this, modelData.timers[i].name, "Timer name"));
    }
    else {
      internalField.Append(new SwitchField<8>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<24>(this, modelData.timers[i].val));
      internalField.Append(new SignedField<24>(this, modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>(this));
      if (IS_TARANIS(board))
        internalField.Append(new ZCharField<8>(this, modelData.timers[i].name, "Timer name"));
      else
        internalField.Append(new ZCharField<3>(this, modelData.timers[i].name, "Timer name"));
    }
  }

  internalField.Append(new UnsignedField<3>(this, modelData.telemetryProtocol));
  internalField.Append(new BoolField<1>(this, modelData.thrTrim));
  internalField.Append(new BoolField<1>(this, modelData.noGlobalFunctions));
  internalField.Append(new UnsignedField<2>(this, modelData.trimsDisplay));
  internalField.Append(new BoolField<1>(this, modelData.frsky.ignoreSensorIds));
  internalField.Append(new SignedField<3>(this, modelData.trimInc));
  internalField.Append(new BoolField<1>(this, modelData.disableThrottleWarning));
  internalField.Append(new BoolField<1>(this, modelData.displayChecklist));
  internalField.Append(new BoolField<1>(this, modelData.extendedLimits));
  internalField.Append(new BoolField<1>(this, modelData.extendedTrims));
  internalField.Append(new BoolField<1>(this, modelData.throttleReversed));
  internalField.Append(new UnsignedField<16>(this, modelData.beepANACenter));

  for (int i=0; i<MAX_MIXERS(board, version); i++)
    internalField.Append(new MixField(this, modelData.mixData[i], board, version, &modelData));
  for (int i=0; i<MAX_CHANNELS(board, version); i++)
    internalField.Append(new LimitField(this, modelData.limitData[i], board, version));
  for (int i=0; i<MAX_EXPOS(board, version); i++)
    internalField.Append(new InputField(this, modelData.expoData[i], board, version));
  internalField.Append(new CurvesField(this, modelData.curves, board, version));
  for (int i=0; i<MAX_LOGICAL_SWITCHES(board, version); i++)
    internalField.Append(new LogicalSwitchField(this, modelData.logicalSw[i], board, version, variant, &modelData));
  for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
    internalField.Append(new ArmCustomFunctionField(this, modelData.customFn[i], board, version, variant));
  }

  internalField.Append(new HeliField(this, modelData.swashRingData, board, version, variant));
  for (int i=0; i<MAX_FLIGHT_MODES(board, version); i++) {
    internalField.Append(new FlightModeField(this, modelData.flightModeData[i], i, board, version));
  }

  internalField.Append(new UnsignedField<8>(this, modelData.thrTraceSrc, "Throttle Source"));

  if (IS_HORUS(board))
    internalField.Append(new SwitchesWarningField<32>(this, modelData.switchWarningStates, board, version));
  else if (IS_TARANIS_X9E(board))
    internalField.Append(new SwitchesWarningField<64>(this, modelData.switchWarningStates, board, version));
  else if (IS_TARANIS(board))
    internalField.Append(new SwitchesWarningField<16>(this, modelData.switchWarningStates, board, version));
  else
    internalField.Append(new SwitchesWarningField<8>(this, modelData.switchWarningStates, board, version));

  if (IS_TARANIS_X9E(board))
    internalField.Append(new UnsignedField<32>(this, modelData.switchWarningEnable));
  else if (!IS_HORUS(board))
    internalField.Append(new UnsignedField<8>(this, modelData.switchWarningEnable));

  for (int i=0; i<MAX_GVARS(board, version); i++) {
    if (version >= 218) {
      internalField.Append(new ZCharField<3>(this, modelData.gvarData[i].name, "GVar name"));
      internalField.Append(new UnsignedField<12>(this, (unsigned &)modelData.gvarData[i].min));
      internalField.Append(new UnsignedField<12>(this, (unsigned &)modelData.gvarData[i].max));
      internalField.Append(new BoolField<1>(this, modelData.gvarData[i].popup));
      internalField.Append(new UnsignedField<1>(this, modelData.gvarData[i].prec));
      internalField.Append(new UnsignedField<2>(this, modelData.gvarData[i].unit));
      internalField.Append(new SpareBitsField<4>(this));
    }
    else {
      internalField.Append(new ZCharField<3>(this, modelData.gvarData[i].name, "GVar name"));
      internalField.Append(new SpareBitsField<3*8>(this));
      internalField.Append(new BoolField<1>(this, modelData.gvarData[i].popup));
      internalField.Append(new SpareBitsField<7>(this));
    }
  }

  // TODO rename to VARIO
  internalField.Append(new FrskyField(this, modelData.frsky, modelData.rssiAlarms, board, version, variant));

  if (version >= 219) {
    // TODO RSSI SOURCE
    internalField.Append(new SpareBitsField<8>(this));

    if (IS_TARANIS_X9(board)) {
      // TODO TOPBAR
      internalField.Append(new SpareBitsField<16>(this));
    }

    // TODO RSSI ALARMS
    internalField.Append(new SpareBitsField<16>(this));
  }

  if (IS_STM32(board) && version <= 218) {
    internalField.Append(new SpareBitsField<3>(this));
    internalField.Append(new UnsignedField<3>(this, modelData.trainerMode));
    internalField.Append(new UnsignedField<2>(this, modelData.potsWarningMode));
  }
  else {
    internalField.Append(new SpareBitsField<6>(this));
    internalField.Append(new UnsignedField<2>(this, modelData.potsWarningMode));
  }

  int modulesCount = (version <= 218 ? 3 : 2);
  for (int module=0; module<modulesCount; module++) {
    internalField.Append(new ConversionField<SignedField<4> >(this, modelData.moduleData[module].protocol, &protocolsConversionTable, "Protocol", DataField::tr("OpenTX doesn't accept this radio protocol")));
    internalField.Append(new SignedField<4>(this, subprotocols[module]));
    internalField.Append(new UnsignedField<8>(this, modelData.moduleData[module].channelsStart));
    internalField.Append(new ConversionField<SignedField<8> >(this, modelData.moduleData[module].channelsCount, -8));
    internalField.Append(new UnsignedField<4>(this, modelData.moduleData[module].failsafeMode));
    internalField.Append(new UnsignedField<3>(this, modelData.moduleData[module].subType));
    internalField.Append(new BoolField<1>(this, modelData.moduleData[module].invertedSerial));
    if (version <= 218) {
      for (int i=0; i<32; i++) {
        internalField.Append(new SignedField<16>(this, modelData.moduleData[module].failsafeChannels[i]));
      }
    }
    internalField.Append(new ConversionField< SignedField<6> >(this, modelData.moduleData[module].ppm.delay, exportPpmDelay, importPpmDelay));
    internalField.Append(new BoolField<1>(this, modelData.moduleData[module].ppm.pulsePol));
    internalField.Append(new BoolField<1>(this, modelData.moduleData[module].ppm.outputType));
    internalField.Append(new SignedField<8>(this, modelData.moduleData[module].ppm.frameLength));
    if (version >= 219) {
      // TODO ACCESS
      internalField.Append(new CharField<1 + 3 * 8 - 2>(this, modelData.moduleData[module].access.data));
    }
  }

  if (version >= 219) {
    for (int i = 0; i < 32; i++) {
      internalField.Append(new SignedField<16>(this, modelData.moduleData[0].failsafeChannels[i]));
    }
  }

  if (version >= 219) {
    // Trainer PPM settings
    internalField.Append(new SpareBitsField<3>(this)); // TODO mode
    internalField.Append(new SpareBitsField<5>(this));
    internalField.Append(new UnsignedField<8>(this, modelData.moduleData[2].channelsStart));
    internalField.Append(new ConversionField<SignedField<8> >(this, modelData.moduleData[2].channelsCount, -8));
    internalField.Append(new SignedField<8>(this, modelData.moduleData[2].ppm.frameLength));
    internalField.Append(new ConversionField<SignedField<6> >(this, modelData.moduleData[2].ppm.delay, exportPpmDelay, importPpmDelay));
    internalField.Append(new BoolField<1>(this, modelData.moduleData[2].ppm.pulsePol));
    internalField.Append(new SpareBitsField<1>(this));
  }

  if (IS_TARANIS(board) && version < 218) {
    for (int i=0; i<MAX_CURVES(board, version); i++) {
      internalField.Append(new ZCharField<6>(this, modelData.curves[i].name, "Curve name"));
    }
  }

  if (IS_STM32(board)) {
    if (version >= 218) {
      for (int i=0; i<MAX_SCRIPTS(board); i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<6>(this, script.filename, true, "Script filename"));
        internalField.Append(new ZCharField<6>(this, script.name, "Script name"));
        for (int j=0; j<6; j++) {
          internalField.Append(new SignedField<16>(this, script.inputs[j]));
        }
      }
    }
    else {
      for (int i=0; i<7; i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<8>(this, script.filename, true, "Script filename"));
        internalField.Append(new ZCharField<8>(this, script.name, "Script name"));
        for (int j=0; j<8; j++) {
          internalField.Append(new SignedField<8>(this, script.inputs[j]));
        }
      }
    }
  }

  for (int i=0; i<32; i++) {
    if (HAS_LARGE_LCD(board))
      internalField.Append(new ZCharField<4>(this, modelData.inputNames[i], "Input name"));
    else
      internalField.Append(new ZCharField<3>(this, modelData.inputNames[i], "Input name"));
  }

  for (int i=0; i<8; i++) {
    if (i < Boards::getCapability(board, Board::Pots) + MAX_SLIDERS(board))
      internalField.Append(new BoolField<1>(this, modelData.potsWarningEnabled[i]));
    else
      internalField.Append(new SpareBitsField<1>(this));
  }

  for (int i=0; i < Boards::getCapability(board, Board::Pots) + MAX_SLIDERS(board); i++) {
    internalField.Append(new SignedField<8>(this, modelData.potPosition[i]));
  }

  if (IS_SKY9X(board)) {
    internalField.Append(new SpareBitsField<8>(this));
    internalField.Append(new SpareBitsField<8>(this));
  }

  for (int i=0; i<MAX_TELEMETRY_SENSORS(board, version); ++i) {
    internalField.Append(new SensorField(this, modelData.sensorData[i], board, version));
  }

  if (IS_TARANIS_X9E(board)) {
    internalField.Append(new UnsignedField<8>(this, modelData.toplcdTimer));
  }

  if (IS_HORUS(board)) {
    for (int i = 0; i < 5; i++) {
      internalField.Append(new CharField<610>(this, modelData.customScreenData[i], false, "Custom screen blob"));
    }
    internalField.Append(new CharField<216>(this, modelData.topbarData, false, "Top bar blob"));
    internalField.Append(new SpareBitsField<8>(this)); // current view
  }
  else if (version >= 219) {
    for (int i = 0; i < 4; i++) {
      internalField.Append(new UnsignedField<2>(this, modelData.frsky.screens[i].type));
    }
    for (int i = 0; i < 4; i++) {
      internalField.Append(new FrskyScreenField(this, modelData.frsky.screens[i], board, version, variant));
    }
  }
}

void OpenTxModelData::beforeExport()
{
  // qDebug() << QString("before export model") << modelData.name;

  for (int module=0; module<2; module++) {
    if ((modelData.moduleData[module].protocol >= PULSES_PXX_XJT_X16 && modelData.moduleData[module].protocol <= PULSES_PXX_XJT_LR12) ||
      modelData.moduleData[module].protocol == PULSES_PXX_R9M) {
      if (modelData.moduleData[module].protocol != PULSES_PXX_R9M) {
        subprotocols[module] = modelData.moduleData[module].protocol - PULSES_PXX_XJT_X16;
      }
      int pxxByte = (modelData.moduleData[module].pxx.power & 0x03)
                    | modelData.moduleData[module].pxx.receiver_telem_off << 4
                    | modelData.moduleData[module].pxx.receiver_channel_9_16 << 5;
      modelData.moduleData[module].ppm.delay = 300 + 50 * pxxByte;
      modelData.moduleData[module].ppm.pulsePol = modelData.moduleData[module].pxx.external_antenna;
      modelData.moduleData[module].ppm.outputType = modelData.moduleData[module].pxx.sport_out;

    }
    else if (modelData.moduleData[module].protocol >= PULSES_LP45 && modelData.moduleData[module].protocol <= PULSES_DSMX) {
      subprotocols[module] = modelData.moduleData[module].protocol - PULSES_LP45;
    }
    else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // copy multi settings to ppm settings to get them written to the eeprom
      // (reverse the int => ms logic of the ppm delay) since only ppm is written
      subprotocols[module] = modelData.moduleData[module].multi.rfProtocol & (0x1f);
      int multiByte = ((modelData.moduleData[module].multi.rfProtocol >> 4) & 0x03) | (modelData.moduleData[module].multi.customProto << 7);
      modelData.moduleData[module].ppm.delay = 300 + 50 * multiByte;
      modelData.moduleData[module].ppm.frameLength = modelData.moduleData[module].multi.optionValue;
      modelData.moduleData[module].ppm.outputType = modelData.moduleData[module].multi.lowPowerMode;
      modelData.moduleData[module].ppm.pulsePol = modelData.moduleData[module].multi.autoBindMode;
    }
    else {
      subprotocols[module] = (module == 0 ? -1 : 0);
    }
  }

  if (IS_HORUS(board)) {
    uint32_t newSwitchWarningStates = 0;
    for (int i = 0; i < MAX_SWITCHES(board, version); i++) {
      uint8_t value = (modelData.switchWarningStates >> (2*i)) & 0x03;
      if (!(modelData.switchWarningEnable & (1 << i))) {
        newSwitchWarningStates |= (value + 1) << (3*i);
      }
    }
    modelData.switchWarningStates = newSwitchWarningStates;
  }
}

void OpenTxModelData::afterImport()
{
  qCDebug(eepromImport) << QString("OpenTxModelData::afterImport()") << modelData.name;

  if (IS_HORUS(board)) {
    modelData.moduleData[0].protocol = PULSES_PXX_XJT_X16;
  }

  for (int module=0; module<2; module++) {
    if (modelData.moduleData[module].protocol == PULSES_PXX_XJT_X16 || modelData.moduleData[module].protocol == PULSES_LP45) {
      if (subprotocols[module] >= 0)
        modelData.moduleData[module].protocol += subprotocols[module];
      else
        modelData.moduleData[module].protocol = PULSES_OFF;
    }
    else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // Copy data from ppm struct to multi struct
      unsigned int multiByte = (unsigned  int)((modelData.moduleData[module].ppm.delay - 300) / 50);
      modelData.moduleData[module].multi.rfProtocol = (subprotocols[module] & 0x0f) | ((multiByte & 0x3) << 4);
      modelData.moduleData[module].multi.customProto = (multiByte & 0x80) == 0x80;
      modelData.moduleData[module].multi.optionValue = modelData.moduleData[module].ppm.frameLength;
      modelData.moduleData[module].multi.lowPowerMode = modelData.moduleData[module].ppm.outputType;
      modelData.moduleData[module].multi.autoBindMode = modelData.moduleData[module].ppm.pulsePol;
    }

    if ((modelData.moduleData[module].protocol >= PULSES_PXX_XJT_X16 && modelData.moduleData[module].protocol <= PULSES_PXX_XJT_LR12) ||
        modelData.moduleData[module].protocol == PULSES_PXX_R9M) {
      // Do the same for pxx
      unsigned int pxxByte = (unsigned  int)((modelData.moduleData[module].ppm.delay - 300) / 50);
      modelData.moduleData[module].pxx.power = pxxByte & 0x03;
      modelData.moduleData[module].pxx.receiver_telem_off = static_cast<bool>(pxxByte & (1 << 4));
      modelData.moduleData[module].pxx.receiver_channel_9_16 = static_cast<bool>(pxxByte & (1 << 5));
      modelData.moduleData[module].pxx.sport_out = modelData.moduleData[module].ppm.outputType;
      modelData.moduleData[module].pxx.external_antenna = modelData.moduleData[module].ppm.pulsePol;
    }
  }

  if (IS_HORUS(board)) {
    uint32_t newSwitchWarningStates = 0;
    for (int i = 0; i < MAX_SWITCHES(board, version); i++) {
      uint8_t value = (modelData.switchWarningStates >> (3*i)) & 0x07;
      if (value == 0)
        modelData.switchWarningEnable |= (1 << i);
      else
        newSwitchWarningStates |= ((value & 0x03) - 1) << (2*i);
    }
    modelData.switchWarningStates = newSwitchWarningStates;
  }
}

OpenTxGeneralData::OpenTxGeneralData(GeneralSettings & generalData, Board::Type board, unsigned int version, unsigned int variant):
  TransformedField(NULL, internalField),
  internalField(this, "General Settings"),
  generalData(generalData),
  board(board),
  version(version),
  inputsCount(CPN_MAX_STICKS + Boards::getCapability(board, Board::Pots) + MAX_SLIDERS(board) + MAX_MOUSE_ANALOGS(board))
{
  qCDebug(eepromImport) << QString("OpenTxGeneralData::OpenTxGeneralData(board: %1, version:%2, variant:%3)").arg(board).arg(version).arg(variant);

  generalData.version = version;
  generalData.variant = variant;

  internalField.Append(new UnsignedField<8>(this, generalData.version));
  internalField.Append(new UnsignedField<16>(this, generalData.variant));

  for (int i=0; i<inputsCount; i++) {
    internalField.Append(new SignedField<16>(this, generalData.calibMid[i]));
    internalField.Append(new SignedField<16>(this, generalData.calibSpanNeg[i]));
    internalField.Append(new SignedField<16>(this, generalData.calibSpanPos[i]));
  }

  internalField.Append(new UnsignedField<16>(this, chkSum));
  if (!IS_HORUS(board)) {
    internalField.Append(new UnsignedField<8>(this, generalData.currModelIndex));
    internalField.Append(new UnsignedField<8>(this, generalData.contrast));
  }
  internalField.Append(new UnsignedField<8>(this, generalData.vBatWarn));
  internalField.Append(new SignedField<8>(this, generalData.txVoltageCalibration));
  internalField.Append(new SignedField<8>(this, generalData.backlightMode));

  for (int i=0; i<CPN_MAX_STICKS; i++) {
    internalField.Append(new SignedField<16>(this, generalData.trainer.calib[i]));
  }
  for (int i=0; i<CPN_MAX_STICKS; i++) {
    internalField.Append(new UnsignedField<6>(this, generalData.trainer.mix[i].src));
    internalField.Append(new UnsignedField<2>(this, generalData.trainer.mix[i].mode));
    internalField.Append(new SignedField<8>(this, generalData.trainer.mix[i].weight));
  }

  internalField.Append(new UnsignedField<8>(this, generalData.view, 0, MAX_VIEWS(board)-1));

  internalField.Append(new SpareBitsField<2>(this)); // TODO buzzerMode?
  internalField.Append(new BoolField<1>(this, generalData.fai));
  internalField.Append(new SignedField<2>(this, (int &)generalData.beeperMode));
  internalField.Append(new BoolField<1>(this, generalData.flashBeep));
  internalField.Append(new BoolField<1>(this, generalData.disableMemoryWarning));
  internalField.Append(new BoolField<1>(this, generalData.disableAlarmWarning));

  internalField.Append(new UnsignedField<2>(this, generalData.stickMode));
  internalField.Append(new SignedField<5>(this, generalData.timezone));
  if (IS_HORUS_OR_TARANIS(board)) {
    internalField.Append(new BoolField<1>(this, generalData.adjustRTC));
  }
  else {
    internalField.Append(new SpareBitsField<1>(this));
  }

  internalField.Append(new UnsignedField<8>(this, generalData.inactivityTimer));
  internalField.Append(new SpareBitsField<3>(this));
  if (IS_HORUS(board))
    internalField.Append(new SpareBitsField<3>(this));
  else if (IS_TARANIS(board))
    internalField.Append(new SignedField<3>(this, generalData.splashDuration));
  else
    internalField.Append(new UnsignedField<3>(this, generalData.splashMode)); // TODO
  internalField.Append(new SignedField<2>(this, (int &)generalData.hapticMode));

  internalField.Append(new SignedField<8>(this, generalData.switchesDelay));

  internalField.Append(new UnsignedField<8>(this, generalData.backlightDelay));
  internalField.Append(new UnsignedField<8>(this, generalData.templateSetup));
  internalField.Append(new SignedField<8>(this, generalData.PPM_Multiplier));
  internalField.Append(new SignedField<8>(this, generalData.hapticLength));

  if (version < 218 || (!IS_TARANIS(board) && !IS_HORUS(board))) {
    internalField.Append(new UnsignedField<8>(this, generalData.reNavigation));
  }

  if (!IS_TARANIS(board) && !IS_HORUS(board)) {
    internalField.Append(new UnsignedField<8>(this, generalData.stickReverse));
  }

  internalField.Append(new SignedField<3>(this, generalData.beeperLength));
  internalField.Append(new SignedField<3>(this, generalData.hapticStrength));
  internalField.Append(new UnsignedField<1>(this, generalData.gpsFormat));
  internalField.Append(new SpareBitsField<1>(this)); // unexpectedShutdown

  internalField.Append(new UnsignedField<8>(this, generalData.speakerPitch));

  internalField.Append(new ConversionField< SignedField<8> >(this, generalData.speakerVolume, -12, 0, 0, 23, "Volume"));

  internalField.Append(new SignedField<8>(this, generalData.vBatMin));
  internalField.Append(new SignedField<8>(this, generalData.vBatMax));

  internalField.Append(new UnsignedField<8>(this, generalData.backlightBright));
  if (version < 218) internalField.Append(new SignedField<8>(this, generalData.txCurrentCalibration));

  if (version < 218) internalField.Append(new SignedField<8>(this, generalData.temperatureWarn));
  if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.mAhWarn));
  if (version < 218) internalField.Append(new UnsignedField<16>(this, generalData.mAhUsed));
  internalField.Append(new UnsignedField<32>(this, generalData.globalTimer));
  if (version < 218) internalField.Append(new SignedField<8>(this, generalData.temperatureCalib));
  internalField.Append(new UnsignedField<4>(this, generalData.bluetoothBaudrate));
  internalField.Append(new UnsignedField<4>(this, generalData.bluetoothMode));
  if (version < 218) internalField.Append(new BoolField<8>(this, generalData.optrexDisplay));
  if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.sticksGain));


  if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.rotarySteps));
  internalField.Append(new UnsignedField<8>(this, generalData.countryCode));
  internalField.Append(new UnsignedField<1>(this, generalData.imperial));
  if (version >= 218) {
    internalField.Append(new BoolField<1>(this, generalData.jitterFilter));
    internalField.Append(new BoolField<1>(this, generalData.disableRssiPoweroffAlarm));
    internalField.Append(new UnsignedField<2>(this, generalData.usbMode));
    internalField.Append(new UnsignedField<2>(this, generalData.jackMode));
    internalField.Append(new SpareBitsField<1>(this));
  }
  else {
    internalField.Append(new SpareBitsField<7>(this));
  }

  internalField.Append(new CharField<2>(this, generalData.ttsLanguage, true, "TTS language"));
  if (version >= 218) {
    internalField.Append(new SignedField<4>(this, generalData.beepVolume));
    internalField.Append(new SignedField<4>(this, generalData.wavVolume));
    internalField.Append(new SignedField<4>(this, generalData.varioVolume));
    internalField.Append(new SignedField<4>(this, generalData.backgroundVolume));
  }
  else {
    internalField.Append(new SignedField<8>(this, generalData.beepVolume));
    internalField.Append(new SignedField<8>(this, generalData.wavVolume));
    internalField.Append(new SignedField<8>(this, generalData.varioVolume));
  }
  internalField.Append(new SignedField<8>(this, generalData.varioPitch));
  internalField.Append(new SignedField<8>(this, generalData.varioRange));
  internalField.Append(new SignedField<8>(this, generalData.varioRepeat));
  if (version < 218) internalField.Append(new SignedField<8>(this, generalData.backgroundVolume));

  if (version >= 218) {
    for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
      internalField.Append(new ArmCustomFunctionField(this, generalData.customFn[i], board, version, variant));
    }
  }

  if (IS_STM32(board)) {
    if (version >= 218) {
      internalField.Append(new UnsignedField<4>(this, generalData.hw_uartMode));
      for (uint8_t i=0; i<4; i++) {
        internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[i]));
      }
    }
    else {
      internalField.Append(new UnsignedField<6>(this, generalData.hw_uartMode));
      if (IS_TARANIS_X9E(board)) {
        internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[2]));
        internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[3]));
      }
      else {
        internalField.Append(new SpareBitsField<2>(this));
      }
    }

    if (IS_HORUS(board)) {
      for (int i=0; i<16; i++) {
        if (i < MAX_SWITCHES(board, version))
          internalField.Append(new UnsignedField<2>(this, generalData.switchConfig[i]));
        else
          internalField.Append(new SpareBitsField<2>(this));
      }
    }
    for (int i=0; i<4; i++) {
      if (i < Boards::getCapability(board, Board::Pots))
        internalField.Append(new UnsignedField<2>(this, generalData.potConfig[i]));
      else
        internalField.Append(new SpareBitsField<2>(this));
    }
    if (!IS_HORUS(board)) {
      internalField.Append(new UnsignedField<8>(this, generalData.backlightColor));
    }
  }
  else if (IS_SKY9X(board) && version >= 218) {
    internalField.Append(new SignedField<8>(this, generalData.txCurrentCalibration));
    internalField.Append(new SignedField<8>(this, generalData.temperatureWarn));
    internalField.Append(new UnsignedField<8>(this, generalData.mAhWarn));
    internalField.Append(new UnsignedField<16>(this, generalData.mAhUsed));
    internalField.Append(new SignedField<8>(this, generalData.temperatureCalib));
    internalField.Append(new BoolField<8>(this, generalData.optrexDisplay));
    internalField.Append(new UnsignedField<8>(this, generalData.sticksGain));
    internalField.Append(new UnsignedField<8>(this, generalData.rotarySteps));
  }

  if (IS_TARANIS_X9E(board))
    internalField.Append(new SpareBitsField<64>(this)); // switchUnlockStates
  else if (IS_TARANIS(board))
    internalField.Append(new SpareBitsField<16>(this)); // switchUnlockStates

  if (version == 217) {
    for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
      internalField.Append(new ArmCustomFunctionField(this, generalData.customFn[i], board, version, variant));
    }
  }

  if (IS_HORUS(board)) {
    for (int i=0; i<MAX_SWITCHES(board, version); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.switchName[i], "Switch name"));
    }
    for (int i=0; i<CPN_MAX_STICKS; ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.stickName[i], "Stick name"));
    }
    for (int i=0; i<Boards::getCapability(board, Board::Pots); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.potName[i], "Pot name"));
    }
    for (int i=0; i<MAX_SLIDERS(board); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.sliderName[i], "Slider name"));
    }
    internalField.Append(new CharField<17>(this, generalData.currModelFilename, true, "Current model filename"));
  }
  else if (IS_TARANIS(board)) {
    for (int i=0; i<MAX_SWITCH_SLOTS(board, version); i++) {
      if (i < MAX_SWITCHES(board, version))
        internalField.Append(new UnsignedField<2>(this, generalData.switchConfig[i]));
      else
        internalField.Append(new SpareBitsField<2>(this));
    }
    for (int i=0; i<MAX_SWITCHES(board, version); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.switchName[i], "Switch name"));
    }
    for (int i=0; i<CPN_MAX_STICKS; ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.stickName[i], "Stick name"));
    }
    for (int i=0; i<Boards::getCapability(board, Board::Pots); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.potName[i], "Pot name"));
    }
    for (int i=0; i<MAX_SLIDERS(board); ++i) {
      internalField.Append(new ZCharField<3>(this, generalData.sliderName[i], "Slider name"));
    }
  }

  if (IS_HORUS(board)) {
    internalField.Append(new SpareBitsField<1>(this));
    internalField.Append(new UnsignedField<7>(this, generalData.backlightOffBright));
    internalField.Append(new ZCharField<10>(this, generalData.bluetoothName, "Bluetooth name"));
  }
  else if (IS_TARANIS_X9E(board) || (version >= 219 && (IS_TARANIS_X7(board) || IS_TARANIS_XLITE(board) || IS_TARANIS_XLITES(board)))) {
    internalField.Append(new SpareBitsField<8>(this));
    internalField.Append(new ZCharField<10>(this, generalData.bluetoothName, "Bluetooth name"));
  }

  if (IS_HORUS(board)) {
    internalField.Append(new CharField<8>(this, generalData.themeName, true, "Theme name"));
    for (int i=0; i<5; i++) {
      internalField.Append(new CharField<8>(this, (char *)generalData.themeOptionValue[i], true, "Theme blob"));
    }
  }

  if (version >= 219) {
    internalField.Append(new ZCharField<8>(this, generalData.registrationId, "ACCESS Registration ID"));
  }

  if (version >= 219 && (IS_TARANIS_XLITES(board) || IS_HORUS(board))) {
    internalField.Append(new SignedField<8>(this, generalData.gyroMax, "Gyro full scale"));
    internalField.Append(new SignedField<8>(this, generalData.gyroOffset, "Gyro Offset"));
  }
}

void OpenTxGeneralData::beforeExport()
{
  uint16_t sum = 0;

  int count = 0;
  for (int i=0; i<inputsCount; i++) {
    sum += generalData.calibMid[i];
    if (++count == 12) break;
    sum += generalData.calibSpanNeg[i];
    if (++count == 12) break;
    sum += generalData.calibSpanPos[i];
    if (++count == 12) break;
  }

  chkSum = sum;
}

void OpenTxGeneralData::afterImport()
{
}
