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

#include "opentxinterface.h"
#include "opentxeeprom.h"
#include "rlefile.h"
#include "appdata.h"
#include "constants.h"

#include <bitset>
#include <QMessageBox>
#include <QTime>
#include <QUrl>

#include "storage.h"  // does this need to be last include?

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)

using namespace Board;

QList<OpenTxEepromInterface *> opentxEEpromInterfaces;

OpenTxEepromInterface::OpenTxEepromInterface(OpenTxFirmware * firmware):
  EEPROMInterface(firmware->getBoard()),
  efile(new RleFile()),
  firmware(firmware)
{
}

OpenTxEepromInterface::~OpenTxEepromInterface()
{
  delete efile;
}

const char * OpenTxEepromInterface::getName()
{
  switch (board) {
    case BOARD_STOCK:
      return "OpenTX for 9X board";
    case BOARD_M128:
      return "OpenTX for M128 / 9X board";
    case BOARD_MEGA2560:
      return "OpenTX for MEGA2560 board";
    case BOARD_GRUVIN9X:
      return "OpenTX for Gruvin9x board / 9X";
    case BOARD_JUMPER_T12:
      return "OpenTX for Jumper T12";
    case BOARD_TARANIS_X9D:
      return "OpenTX for FrSky Taranis X9D";
    case BOARD_TARANIS_X9DP:
      return "OpenTX for FrSky Taranis X9D+";
    case BOARD_TARANIS_X9E:
      return "OpenTX for FrSky Taranis X9E";
    case BOARD_TARANIS_X7:
      return "OpenTX for FrSky Taranis X7";
    case BOARD_TARANIS_X9LITE:
      return "OpenTX for FrSky Taranis X9-Lite";
    case BOARD_TARANIS_XLITE:
      return "OpenTX for FrSky Taranis X-Lite";
    case BOARD_TARANIS_XLITES:
      return "OpenTX for FrSky Taranis X-Lite S/Pro";
    case BOARD_SKY9X:
      return "OpenTX for Sky9x board / 9X";
    case BOARD_9XRPRO:
      return "OpenTX for 9XR-PRO";
    case BOARD_AR9X:
      return "OpenTX for ar9x board / 9X";
    case BOARD_X12S:
      return "OpenTX for FrSky Horus";
    case BOARD_X10:
      return "OpenTX for FrSky X10";
    default:
      return "OpenTX for an unknown board";
  }
}

bool OpenTxEepromInterface::loadRadioSettingsFromRLE(GeneralSettings & settings, RleFile * rleFile, uint8_t version)
{
  QByteArray data(sizeof(settings), 0); // GeneralSettings should be always bigger than the EEPROM struct
  OpenTxGeneralData open9xSettings(settings, board, version);
  efile->openRd(FILE_GENERAL);
  int size = rleFile->readRlc2((uint8_t *)data.data(), data.size());
  if (size) {
    open9xSettings.Import(data);
    return checkVariant(settings.version, settings.variant);
  }
  else {
    qWarning() << " error when loading general settings";
    return false;
  }
}

bool OpenTxEepromInterface::loadModelFromRLE(ModelData & model, RleFile * rleFile, unsigned int index, uint8_t version, uint32_t variant)
{
  QByteArray data(sizeof(model), 0);  // ModelData should be always bigger than the EEPROM struct
  rleFile->openRd(FILE_MODEL(index));
  int size = rleFile->readRlc2((uint8_t *)data.data(), data.size());
  if (size) {
    if (loadFromByteArray<ModelData, OpenTxModelData>(model, data, version, variant)) {
      model.used = true;
    }
  }
  else {
    model.clear();
  }
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::saveToByteArray(const T & src, QByteArray & data, uint8_t version)
{
  if (version == 0) {
    version = getLastDataVersion(getBoard());
  }
  QByteArray raw;
  T srcCopy(src); // work on a copy of radio data, because Export() will modify it!
  M manager(srcCopy, board, version, 0);
  // manager.dump();
  manager.Export(raw);
  data.resize(8);
  *((uint32_t*)&data.data()[0]) = Boards::getFourCC(board);
  data[4] = version;
  data[5] = 'M';
  *((uint16_t*)&data.data()[6]) = raw.size();
  data.append(raw);
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::loadFromByteArray(T & dest, const QByteArray & data, uint8_t version, uint32_t variant)
{
  M manager(dest, board, version, variant);
  if (manager.Import(data) != 0) {
    return false;
  }
  // manager.dump(); // Dumps the structure so that it's easy to check with firmware datastructs.h
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::loadFromByteArray(T & dest, const QByteArray & data)
{
  uint32_t fourcc = *((uint32_t*)&data.data()[0]);
  if (Boards::getFourCC(board) != fourcc) {
    if (IS_HORUS(board) && fourcc == 0x3178396F) {
      qDebug() << QString().sprintf("%s: Deprecated fourcc used %x vs %x", getName(), fourcc, Boards::getFourCC(board));
    }
    else {
      qDebug() << QString().sprintf("%s: Wrong fourcc %x vs %x", getName(), fourcc, Boards::getFourCC(board));
      return false;
    }
  }
  qDebug() << QString().sprintf("%s: OK", getName());
  uint8_t version = data[4];
  QByteArray raw = data.right(data.size() - 8);
  return loadFromByteArray<T, M>(dest, raw, version);
}

unsigned long OpenTxEepromInterface::load(RadioData &radioData, const uint8_t * eeprom, int size)
{
  QDebug dbg = qDebug();
  dbg.setAutoInsertSpaces(false);
  dbg << "trying " << getName() << " import...";

  std::bitset<NUM_ERRORS> errors;

  if (size != Boards::getEEpromSize(board)) {
    if (size == 4096) {
      int notnull = false;
      for (int i = 2048; i < 4096; i++) {
        if (eeprom[i] != 255) {
          notnull = true;
        }
      }
      if (notnull) {
        dbg << " wrong size (" << size << ")";
        errors.set(WRONG_SIZE);
        return errors.to_ulong();
      }
      else {
        errors.set(HAS_WARNINGS);
        errors.set(WARNING_WRONG_FIRMWARE);
        size = 2048;
      }
    }
    else {
      dbg << " wrong size (" << size << "/" << Boards::getEEpromSize(board) << ")";
      errors.set(WRONG_SIZE);
      return errors.to_ulong();
    }
  }

  if (!efile->EeFsOpen((uint8_t *) eeprom, size, board)) {
    dbg << " wrong file system";
    errors.set(WRONG_FILE_SYSTEM);
    return errors.to_ulong();
  }

  efile->openRd(FILE_GENERAL);

  uint8_t version;
  if (efile->readRlc2(&version, 1) != 1) {
    dbg << " no";
    errors.set(UNKNOWN_ERROR);
    return errors.to_ulong();
  }

  dbg << " version " << (unsigned int) version;

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
  }
  else if (version_error == NOT_OPENTX) {
    dbg << " not open9x";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (!loadRadioSettingsFromRLE(radioData.generalSettings, efile, version)) {
    dbg << " ko";
    errors.set(UNKNOWN_ERROR);
    return errors.to_ulong();
  }

  dbg << " variant " << radioData.generalSettings.variant;
  if (getCurrentFirmware()->getCapability(Models) == 0) {
    radioData.models.resize(firmware->getCapability(Models));
  }
  for (int i = 0; i < firmware->getCapability(Models); i++) {
    if (i < (int)radioData.models.size() && !loadModelFromRLE(radioData.models[i], efile, i, version, radioData.generalSettings.variant)) {
      dbg << " ko";
      errors.set(UNKNOWN_ERROR);
      if (getCurrentFirmware()->getCapability(Models) == 0) {
        radioData.models.resize(i);
      }
      return errors.to_ulong();
    }
  }
  dbg << " ok";
  errors.set(ALL_OK);
  return errors.to_ulong();
}

uint8_t OpenTxEepromInterface::getLastDataVersion(Board::Type board)
{
  switch (board) {
    case BOARD_STOCK:
      return 216;
    case BOARD_GRUVIN9X:
    case BOARD_MEGA2560:
    case BOARD_M128:
      return 217;
    default:
      return 219;
  }
}

void OpenTxEepromInterface::showErrors(const QString & title, const QStringList & errors)
{
  QString msg;
  if (errors.empty()) {
    msg = tr("Unknown error");
  }
  else {
    int noErrorsToDisplay = std::min((int)errors.size(), 10);
    for (int n = 0; n < noErrorsToDisplay; n++) {
      msg += " -" + errors.at(n) + "\n";
    }
    if (noErrorsToDisplay < errors.size()) {
      msg += tr(" ... plus %1 errors").arg(errors.size() - noErrorsToDisplay) + "\n" + msg;
    }
  }

  QMessageBox::warning(NULL,
                       CPN_STR_TTL_ERROR,
                       title + "\n" + msg);
}

int OpenTxEepromInterface::save(uint8_t * eeprom, const RadioData & radioData, uint8_t version, uint32_t variant)
{
  // TODO QMessageBox::warning should not be called here

  qDebug() << "ICI";

  if (version == 0) {
    version = getLastDataVersion(board);
  }

  int size = Boards::getEEpromSize(board);

  efile->EeFsCreate(eeprom, size, board, version);

  if (board == BOARD_M128) {
    variant |= M128_VARIANT;
  }
  else if (IS_TARANIS_X9E(board)) {
    variant |= TARANIS_X9E_VARIANT;
  }
  else if (IS_TARANIS_X9LITE(board)) {
    variant |= TARANIS_X9LITE_VARIANT;
  }
  else if (IS_TARANIS_X7(board)) {
    variant |= TARANIS_X7_VARIANT;
  }
  else if (IS_TARANIS_XLITES(board)) {
    variant |= TARANIS_XLITES_VARIANT;
  }
  else if (IS_TARANIS_XLITE(board)) {
    variant |= TARANIS_XLITE_VARIANT;
  }
  else if (IS_JUMPER_T12(board)) {
    variant |= JUMPER_T12_VARIANT;
  }

  OpenTxGeneralData generator((GeneralSettings &)radioData.generalSettings, board, version, variant);
  // generator.dump();
  QByteArray data;
  generator.Export(data);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (const uint8_t *)data.constData(), data.size());
  if (sz == 0 || generator.errors().count() > 0) {
    showErrors(tr("Cannot write radio settings"), generator.errors());
    return 0;
  }

  for (int i = 0; i < getCurrentFirmware()->getCapability(Models); i++) {
    if (i < (int)radioData.models.size() && !radioData.models[i].isEmpty()) {
      OpenTxModelData generator((ModelData &)radioData.models[i], board, version, variant);
      // generator.dump();
      QByteArray data;
      generator.Export(data);
      int sz = efile->writeRlc2(FILE_MODEL(i), FILE_TYP_MODEL, (const uint8_t *)data.constData(), data.size());
      if (sz == 0 || generator.errors().count() > 0) {
        showErrors(tr("Cannot write model %1").arg(radioData.models[i].name), generator.errors());
        return 0;
      }
    }
  }

  return size;
}

int OpenTxEepromInterface::getSize(const ModelData &model)
{
  if (IS_SKY9X(board))
    return 0;

  if (model.isEmpty())
    return 0;

  QByteArray tmp(Boards::getEEpromSize(Board::BOARD_UNKNOWN), 0);
  efile->EeFsCreate((uint8_t *) tmp.data(), Boards::getEEpromSize(Board::BOARD_UNKNOWN), board, 255/*version max*/);

  ModelData srcCopy(model); // work on a copy of model data, because Export() may modify it!
  OpenTxModelData open9xModel(srcCopy, board, 255/*version max*/, getCurrentFirmware()->getVariantNumber());

  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_MODEL, (const uint8_t *) eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

int OpenTxEepromInterface::getSize(const GeneralSettings &settings)
{
  if (IS_SKY9X(board))
    return 0;

  QByteArray tmp(Boards::getEEpromSize(Board::BOARD_UNKNOWN), 0);
  efile->EeFsCreate((uint8_t *) tmp.data(), Boards::getEEpromSize(Board::BOARD_UNKNOWN), board, 255);

  GeneralSettings srcCopy(settings); // work on a copy of settings data, because Export() may modify it!
  OpenTxGeneralData open9xGeneral(srcCopy, board, 255, getCurrentFirmware()->getVariantNumber());
  // open9xGeneral.dump();

  QByteArray eeprom;
  open9xGeneral.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_GENERAL, (const uint8_t *) eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

Firmware * OpenTxFirmware::getFirmwareVariant(const QString &id)
{
  if (id == getId()) {
    return this;
  }
  else if (id.contains(getId() + "-") || (!id.contains("-") && id.contains(getId()))) {
    Firmware * result = new OpenTxFirmware(id, this);
    // TODO result.variant = firmware->getVariant(id);
    return result;
  }
  else {
    return NULL;
  }
}

int OpenTxFirmware::getCapability(::Capability capability)
{
  switch (capability) {
    case Models:
      if (IS_HORUS(board))
        return 0;
      else if (IS_ARM(board))
        return 60;
      else if (board == BOARD_M128)
        return 30;
      else if (IS_2560(board))
        return 30;
      else
        return 16;
    case Imperial:
      if (IS_ARM(board))
        return 0;
      else
        return id.contains("imperial") ? 1 : 0;
    case ModelImage:
      return (board == BOARD_TARANIS_X9D || IS_TARANIS_PLUS(board) || IS_HORUS(board));
    case HasBeeper:
      return (!IS_ARM(board));
    case HasPxxCountry:
      return 1;
    case HasGeneralUnits:
      if (IS_ARM(board))
        return 1;
      else
        return 0;
    case HasNegAndSwitches:
      return IS_ARM(board);
    case PPMExtCtrl:
      return 1;
    case PPMFrameLength:
      return 40;
    case FlightModes:
      if (IS_ARM(board))
        return 9;
      else if (IS_2560(board))
        return 6;
      else
        return 5;
    case FlightModesHaveFades:
      return 1;
    case Heli:
      if (IS_HORUS_OR_TARANIS(board))
        return id.contains("noheli") ? 0 : 1;
      else
        return id.contains("heli") ? 1 : 0;
    case Gvars:
      if (IS_HORUS_OR_TARANIS(board))
        return id.contains("nogvars") ? 0 : 9;
      else if (id.contains("gvars"))
        return IS_ARM(board) ? 9 : 5;
      else
        return 0;
    case ModelName:
      return (IS_HORUS(board) ? 15 : (HAS_LARGE_LCD(board) ? 12 : 10));
    case FlightModesName:
      return (IS_HORUS_OR_TARANIS(board) ? 10 : 6);
    case GvarsName:
      return (IS_9X(board) ? 0 : 3);
    case GvarsInCS:
    case HasFAIMode:
      return 1;
    case GvarsAreNamed:
    case GvarsFlightModes:
      return ((IS_ARM(board) || IS_2560(board)) ? 1 : 0);
    case Mixes:
      return (IS_ARM(board) ? 64 : 32);
    case OffsetWeight:
      return (IS_ARM(board) ? 500 : 245);
    case Timers:
      return (IS_ARM(board) ? 3 : 2);
    case TimersName:
      if (HAS_LARGE_LCD(board))
        return 8;
      else if (IS_ARM(board))
        return 3;
      else
        return 0;
    case PermTimers:
      return (IS_2560(board) || IS_ARM(board));
    case CustomFunctions:
      if (IS_ARM(board))
        return 64;
      else if (IS_2560(board) || board == BOARD_M128)
        return 24;
      else
        return 16;
    case SafetyChannelCustomFunction:
      return id.contains("nooverridech") ? 0 : 1;
    case LogicalSwitches:
      if (IS_ARM(board))
        return 64;
      else
        return 12;
    case CustomAndSwitches:
      if (IS_ARM(board))
        return getCapability(LogicalSwitches);
      else
        return 15/*4bits*/- 9/*sw positions*/;
    case LogicalSwitchesExt:
      return (IS_ARM(board) ? true : false);
    case RotaryEncoders:
      if (board == BOARD_GRUVIN9X)
        return 2;
      else if (IS_SKY9X(board))
        return 1;
      else
        return 0;
    case Outputs:
      return (IS_ARM(board) ? 32 : 16);
    case NumCurvePoints:
      return (IS_ARM(board) ? 512 : 104);
    case VoicesAsNumbers:
      return (IS_ARM(board) ? 0 : 1);
    case VoicesMaxLength:
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : 0);
    case MultiLangVoice:
      return (IS_ARM(board) ? 1 : 0);
    case SoundPitch:
      return 1;
    case Haptic:
      return (IS_2560(board) || IS_SKY9X(board) || IS_TARANIS_PLUS(board) || IS_TARANIS_SMALL(board) || IS_TARANIS_X9E(board) || IS_HORUS(board) || IS_JUMPER_T12(board) || id.contains("haptic"));
    case ModelTrainerEnable:
      if (IS_HORUS_OR_TARANIS(board) && board!=Board::BOARD_TARANIS_XLITE)
        return 1;
      else
        return 0;
    case MaxVolume:
      return (IS_ARM(board) ? 23 : 7);
    case MaxContrast:
      if (IS_TARANIS_SMALL(board) || IS_JUMPER_T12(board))
        return 30;
      else
        return 45;
    case MinContrast:
      if (IS_TARANIS_X9(board))
        return 0;
      else
        return 10;
    case HasSoundMixer:
      return (IS_ARM(board) ? 1 : 0);
    case ExtraInputs:
      return 1;
    case TrimsRange:
      return 125;
    case ExtendedTrimsRange:
      return 500;
    case Simulation:
      return 1;
    case NumCurves:
      return (HAS_LARGE_LCD(board) ? 32 : (IS_ARM(board) ? 16 : 8));
    case HasMixerNames:
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : false);
    case HasExpoNames:
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : false);
    case HasNoExpo:
      return (IS_HORUS_OR_TARANIS(board) ? false : true);
    case ChannelsName:
      return (IS_ARM(board) ? (HAS_LARGE_LCD(board) ? 6 : 4) : 0);
    case HasCvNames:
      return (IS_ARM(board) ? 1 : 0);
    case Telemetry:
      if (IS_2560(board) || IS_ARM(board) || id.contains("frsky") || id.contains("telemetrez"))
        return TM_HASTELEMETRY | TM_HASOFFSET | TM_HASWSHH;
      else
        return 0;
    case TelemetryBars:
      return 1;
    case TelemetryCustomScreens:
      if (IS_HORUS(board))
        return 0;
      else
        return IS_ARM(board) ? 4 : 2;
    case TelemetryCustomScreensFieldsPerLine:
      return HAS_LARGE_LCD(board) ? 3 : 2;
    case NoTelemetryProtocol:
      return IS_HORUS_OR_TARANIS(board) ? 1 : 0;
    case TelemetryUnits:
      return 0;
    case TelemetryMaxMultiplier:
      return (IS_ARM(board) ? 32 : 8);
    case PPMCenter:
      return (IS_HORUS_OR_TARANIS(board) ? 500 : (id.contains("ppmca") ? 125 : 0));
    case PPMUnitMicroseconds:
      return id.contains("ppmus") ? 1 : 0;
    case SYMLimits:
      return 1;
    case OptrexDisplay:
      return (board == BOARD_SKY9X ? true : false);
    case HasVario:
      return 1;
    case HasVarioSink:
      return ((IS_2560(board) || IS_ARM(board)) ? true : false);
    case HasFailsafe:
      return (IS_ARM(board) ? 32 : 0);
    case NumModules:
      return (IS_ARM(board) ? 2 : 1);
    case NumFirstUsableModule:
      return (IS_JUMPER_T12(board) ? 1 : 0);
    case HasModuleR9MFlex:
      return id.contains("flexr9m");
    case HasModuleR9MMini:
      return IS_TARANIS_XLITE(board) && !id.contains("stdr9m");
    case HasPPMStart:
      return (IS_ARM(board) ? true : false);
    case HastxCurrentCalibration:
      return (IS_SKY9X(board) ? true : false);
    case HasVolume:
      return (IS_ARM(board) ? true : false);
    case HasBrightness:
      return (IS_ARM(board) ? true : false);
    case PerModelTimers:
      return 1;
    case SlowScale:
      return (IS_ARM(board) ? 10 : 2);
    case SlowRange:
      return (IS_ARM(board) ? 250 : 15);
    case CSFunc:
      return 18;
    case HasSDLogs:
      return ((IS_2560(board) || IS_ARM(board)) ? true : false);
    case LcdWidth:
      if (IS_HORUS(board))
        return 480;
      else if (IS_TARANIS_SMALL(board) || IS_JUMPER_T12(board))
        return 128;
      else if (IS_TARANIS(board))
        return 212;
      else
        return 128;
    case LcdHeight:
      if (IS_HORUS(board))
        return 272;
      else
        return 64;
    case LcdDepth:
      if (IS_HORUS(board))
        return 16;
      else if (IS_TARANIS_SMALL(board) || IS_JUMPER_T12(board))
        return 1;
      else if (IS_TARANIS(board))
        return 4;
      else
        return 1;
    case GetThrSwitch:
      return (IS_HORUS_OR_TARANIS(board) ? SWITCH_SF1 : SWITCH_THR);
    case HasDisplayText:
      return IS_ARM(board) ? 1 : 0;
    case HasTopLcd:
      return IS_TARANIS_X9E(board) ? 1 : 0;
    case GlobalFunctions:
      return IS_ARM(board) ? 64 : 0;
    case VirtualInputs:
      return IS_ARM(board) ? 32 : 0;
    case InputsLength:
      return HAS_LARGE_LCD(board) ? 4 : 3;
    case TrainerInputs:
      return IS_ARM(board) ? 16 : 8;
    case RtcTime:
      return IS_ARM(board) || IS_2560(board) ? 1 : 0;
    case LuaScripts:
      return IS_HORUS_OR_TARANIS(board) && id.contains("lua") ? 7 : 0;
    case LuaInputsPerScript:
      return IS_HORUS_OR_TARANIS(board) ? 10 : 0;
    case LuaOutputsPerScript:
      return IS_HORUS_OR_TARANIS(board) ? 6 : 0;
    case LimitsPer1000:
    case EnhancedCurves:
      return IS_ARM(board);
    case HasFasOffset:
      return (IS_STOCK(board) ? false : true);
    case HasMahPersistent:
      return (IS_ARM(board) ? true : false);
    case SimulatorVariant:
      if (board == BOARD_STOCK)
        return SIMU_STOCK_VARIANTS;
      else if (board == BOARD_M128)
        return SIMU_M128_VARIANTS;
      else if (IS_TARANIS_X9E(board))
        return TARANIS_X9E_VARIANT;
      else if (IS_TARANIS_X9LITE(board))
        return TARANIS_X9LITE_VARIANT;
      else if (IS_TARANIS_X7(board))
        return TARANIS_X7_VARIANT;
      else if (IS_TARANIS_XLITES(board))
        return TARANIS_XLITES_VARIANT;
      else if (IS_TARANIS_XLITE(board))
        return TARANIS_XLITE_VARIANT;
      else if (IS_JUMPER_T12(board))
        return JUMPER_T12_VARIANT;
      else
        return 0;
    case MavlinkTelemetry:
      return id.contains("mavlink") ? 1 : 0;
    case SportTelemetry:
      return IS_ARM(board) ? 1 : 0;
    case HasInputDiff:
    case HasMixerExpo:
      return (IS_HORUS_OR_TARANIS(board) ? true : false);
    case HasBatMeterRange:
      return (IS_HORUS_OR_TARANIS(board) ? true : id.contains("battgraph"));
    case DangerousFunctions:
      return id.contains("danger") ? 1 : 0;
    case HasModelCategories:
      return IS_HORUS(board);
    case HasSwitchableJack:
      return IS_TARANIS_XLITES(board);
    default:
      return 0;
  }
}

QString OpenTxFirmware::getAnalogInputName(unsigned int index)
{
  return Boards::getAnalogInputName(board, index);
}

QTime OpenTxFirmware::getMaxTimerStart()
{
  if (IS_HORUS_OR_TARANIS(board))
    return QTime(23, 59, 59);
  else if (IS_ARM(board))
    return QTime(8, 59, 59);
  else
    return QTime(0, 59, 59);
}

int OpenTxFirmware::isAvailable(PulsesProtocol proto, int port)
{
  if (IS_HORUS_OR_TARANIS(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case PULSES_OFF:
            return 1;
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_LR12:
            return (IS_TARANIS_XLITES(board) || IS_TARANIS_X9LITE(board)) ? 0 : 1;
          case PULSES_PXX_XJT_D8:
            return (IS_TARANIS_XLITES(board) || IS_TARANIS_X9LITE(board) || id.contains("eu")) ? 0 : 1;
          case PULSES_PPM:
            return id.contains("internalppm") ? 1 : 0;
          case PULSES_ACCESS_ISRM:
            return (IS_TARANIS_XLITES(board) || IS_TARANIS_X9LITE(board)) ? 1 : 0;
          default:
            return 0;
        }

      case 1:
        switch (proto) {
          case PULSES_OFF:
          case PULSES_PPM:
            return 1;
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_D8:
          case PULSES_PXX_XJT_LR12:
            return (IS_TARANIS_XLITES(board) || IS_TARANIS_X9LITE(board)) ? 0 : 1;
          case PULSES_PXX_R9M:
          case PULSES_LP45:
          case PULSES_DSM2:
          case PULSES_DSMX:
          case PULSES_SBUS:
            return 1;
          case PULSES_MULTIMODULE:
            if(IS_JUMPER_T12(board))
              return 1;
            else
              return id.contains("multimodule") ? 1 : 0;
          case PULSES_CROSSFIRE:
            return id.contains("crossfire") ? 1 : 0;
          case PULSES_ACCESS_R9M_LITE:
          case PULSES_ACCESS_R9M_LITE_PRO:
            return (IS_TARANIS_XLITES(board) || IS_TARANIS_X9LITE(board)) ? 1 : 0;
          default:
            return 0;
        }

      case -1:
        switch (proto) {
          case PULSES_PPM:
            return 1;
          default:
            return 0;
        }

      default:
        return 0;
    }
  }
  else if (IS_SKY9X(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case PULSES_PPM:
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_D8:
          case PULSES_PXX_XJT_LR12:
          case PULSES_PXX_R9M:
          case PULSES_LP45:
          case PULSES_DSM2:
          case PULSES_DSMX:
          case PULSES_SBUS:
            return 1;
          case PULSES_MULTIMODULE:
            return id.contains("multimodule") ? 1 : 0;
          default:
            return 0;
        }
        break;
      case 1:
        switch (proto) {
          case PULSES_PPM:
            return 1;
          default:
            return 0;
        }
        break;
      default:
        return 0;
    }
  }
  else {
    switch (proto) {
      case PULSES_PPM:
      case PULSES_DSMX:
      case PULSES_LP45:
      case PULSES_DSM2:
        // case PULSES_PXX_DJT:     // Unavailable for now
      case PULSES_PPM16:
      case PULSES_PPMSIM:
        return 1;
      default:
        return 0;
    }
  }
}

template<typename T, size_t SIZE>
size_t getSizeA(T (&)[SIZE])
{
  return SIZE;
}

EepromLoadErrors OpenTxEepromInterface::checkVersion(unsigned int version)
{
  switch (version) {
    case 201:
      // first version
    case 202:
      // channel order is now always RUD - ELE - THR - AIL
      // changes in timers
      // ppmFrameLength added
      // thrTraceSrc added
    case 203:
      // mixers changed (for the trims use for change the offset of a mix)
      // telemetry offset raised to -127 +127
      // function switches now have a param on 4 bits
    case 204:
      // telemetry changes (bars)
    case 205:
      // mixer changes (differential, negative curves)...
      // case 206:
    case 207:
      // V4: Rotary Encoders position in FlightModes
    case 208:
      // Trim value in 16bits
      // FrSky A1/A2 offset on 12bits
      // ARM: More Mixers / Expos / CSW / FSW / CHNOUT
    case 209:
      // Add TrmR, TrmE, TrmT, TrmA as Mix sources
      // Trims are now OFF / ON / Rud / Ele / Thr / Ail
    case 210:
      // Add names in Mixes / Expos
      // Add a new telemetry screen
      // Add support for Play Track <filename>
    case 211:
      // Curves big change
    case 212:
      // Big changes in mixers / limitse
    case 213:
      // GVARS / Variants introduction
    case 214:
      // Massive EEPROM change!
    case 215:
      // M128 revert because too much RAM used!
    case 216:
      // A lot of things (first github release)
      if (IS_2560(board) || IS_M128(board)) {
        return OLD_VERSION;
      }
    case 217:
      // 3 logical switches removed on M128 / gruvin9x boards
      if (IS_ARM(board)) {
        return OLD_VERSION;
      }
    case 218:
      // ACCESS
      // switches add for X7/X10/X12S
      // 60 sensors for X12
      return OLD_VERSION;

    case 219:
      break;

    default:
      return NOT_OPENTX;
  }

  return ALL_OK;
}

bool OpenTxEepromInterface::checkVariant(unsigned int version, unsigned int variant)
{
  bool variantError = false;
  if (board == BOARD_M128 && !(variant & M128_VARIANT)) {
    if (version == 212) {
      uint8_t tmp[1000];
      for (int i = 1; i < 31; i++) {
        efile->openRd(i);
        int sz = efile->readRlc2(tmp, sizeof(tmp));
        if (sz == 849) {
          qWarning() << " warning: M128 variant not set (but model size seems ok)";
          return true;
        }
      }
    }
    variantError = true;
  }
  else if (IS_TARANIS_X9E(board)) {
    if (variant != TARANIS_X9E_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_TARANIS_X7(board)) {
    if (variant != TARANIS_X7_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_TARANIS_XLITES(board)) {
    if (variant != TARANIS_XLITES_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_TARANIS_XLITE(board)) {
    if (variant != TARANIS_XLITE_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_TARANIS_X9LITE(board)) {
    if (variant != TARANIS_X9LITE_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_JUMPER_T12(board)) {
    if (variant != JUMPER_T12_VARIANT) {
      variantError = true;
    }
  }
  else if (IS_TARANIS(board)) {
    if (variant != 0) {
      variantError = true;
    }
  }

  if (variantError) {
    qWarning() << " wrong variant (" << variant << ")";
    return false;
  }

  return true;
}

bool
OpenTxEepromInterface::loadModelFromBackup(ModelData & model, const uint8_t * data, unsigned int size, uint8_t version, uint32_t variant)
{
  QByteArray backupData((char *) data, size);
  QByteArray modelData;
  if (IS_SKY9X(board)) {
    modelData = QByteArray((char *) data, size);
  }
  else {
    importRlc(modelData, backupData);
  }
  if (modelData.size()) {
    if (loadFromByteArray<ModelData, OpenTxModelData>(model, modelData, version, variant)) {
      model.used = true;
      return true;
    }
  }
  model.clear();
  return false;
}

unsigned long OpenTxEepromInterface::loadBackup(RadioData &radioData, const uint8_t * eeprom, int esize, int index)
{
  std::bitset<NUM_ERRORS> errors;

  QDebug dbg = qDebug();
  dbg.setAutoInsertSpaces(false);
  dbg << "trying " << getName() << " backup import...";

  if (esize < 8 || memcmp(eeprom, "o9x", 3) != 0) {
    dbg << " no\n";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  Type backupBoard = (Type) -1;
  switch (eeprom[3]) {
    case 0x33:
      backupBoard = BOARD_TARANIS_X9D;
      break;
    case 0x32:
      backupBoard = BOARD_SKY9X;
      break;
    case 0x31:
      backupBoard = BOARD_GRUVIN9X;
      break;
    default:
      dbg << " unknown board";
      errors.set(UNKNOWN_BOARD);
      return errors.to_ulong();
  }

  if (backupBoard != board) {
    dbg << " not right board";
    errors.set(WRONG_BOARD);
    return errors.to_ulong();
  }

  uint8_t version = eeprom[4];
  uint8_t bcktype = eeprom[5];
  uint16_t size = ((uint16_t) eeprom[7] << 8) + eeprom[6];
  uint16_t variant = ((uint16_t) eeprom[9] << 8) + eeprom[8];

  dbg << " version " << (unsigned int) version << " ";

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
  }
  else if (version_error == NOT_OPENTX) {
    dbg << " not open9x";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (size > esize - 8) {
    dbg << " wrong size";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  if (bcktype == 'M') {
    if (!loadModelFromBackup(radioData.models[index], &eeprom[8], size, version, variant)) {
      dbg << " ko";
      errors.set(UNKNOWN_ERROR);
      return errors.to_ulong();
    }
  }
  else {
    dbg << " backup type not supported";
    errors.set(BACKUP_NOT_SUPPORTED);
    return errors.to_ulong();
  }

  dbg << " ok";
  errors.set(ALL_OK);
  return errors.to_ulong();
}

QString OpenTxFirmware::getFirmwareBaseUrl()
{
  return g.openTxCurrentDownloadBranchUrl() % QStringLiteral("firmware/");
}

QString OpenTxFirmware::getFirmwareUrl()
{
  return getFirmwareBaseUrl() % QString("getfw.php?fw=%1.bin").arg(QString(QUrl::toPercentEncoding(id)));
}

QString OpenTxFirmware::getReleaseNotesUrl()
{
  return getFirmwareBaseUrl() % QStringLiteral("releasenotes.txt");
}

QString OpenTxFirmware::getStampUrl()
{
  return getFirmwareBaseUrl() % QStringLiteral("stamp-opentx.txt");
}


// Firmware registrations
// NOTE: "recognized" build options are defined in /radio/util/fwoptions.py

void registerOpenTxFirmware(OpenTxFirmware * firmware, bool deprecated = false)
{
  OpenTxEepromInterface * eepromInterface = new OpenTxEepromInterface(firmware);
  firmware->setEEpromInterface(eepromInterface);
  opentxEEpromInterfaces.push_back(eepromInterface);
  eepromInterfaces.push_back(eepromInterface);
  if (!deprecated)
    Firmware::addRegisteredFirmware(firmware);
}

void addOpenTxCommonOptions(OpenTxFirmware * firmware)
{
  static const Firmware::OptionsGroup fai_options = {
    Firmware::Option("faichoice", Firmware::tr("Possibility to enable FAI MODE (no telemetry) at field")),
    Firmware::Option("faimode",   Firmware::tr("FAI MODE (no telemetry) always enabled"))
  };
  firmware->addOption("ppmus", Firmware::tr("Channel values displayed in us"));
  firmware->addOptionsGroup(fai_options);
  firmware->addOption("nooverridech", Firmware::tr("No OverrideCH functions available"));
}

void addOpenTxRfOptions(OpenTxFirmware * firmware, bool flex = true)
{
  static const Firmware::Option opt_eu("eu", Firmware::tr("Removes D8 FrSky protocol support which is not legal for use in the EU on radios sold after Jan 1st, 2015"));
  static const Firmware::Option opt_fl("flexr9m", Firmware::tr("Enable non certified firmwares"));
  firmware->addOption("multimodule", Firmware::tr("Support for the DIY-Multiprotocol-TX-Module"));
  if (flex)
    firmware->addOptionsGroup({opt_eu, opt_fl});
  else
    firmware->addOption(opt_eu);
}

void addOpenTxFontOptions(OpenTxFirmware * firmware)
{
  firmware->addOption("sqt5font", Firmware::tr("Use alternative SQT5 font"));
}

void addOpenTxFrskyOptions(OpenTxFirmware * firmware)
{
  addOpenTxCommonOptions(firmware);
  firmware->addOption("noheli", Firmware::tr("Disable HELI menu and cyclic mix support"));
  firmware->addOption("nogvars", Firmware::tr("Disable Global variables"));
  firmware->addOption("lua", Firmware::tr("Enable Lua custom scripts screen"));
  firmware->addOption("luac", Firmware::tr("Enable Lua compiler"));
  firmware->addOption("crossfire", Firmware::tr("Support for Crossfire TX Module"));
  addOpenTxRfOptions(firmware);
}

void addOpenTxTaranisOptions(OpenTxFirmware * firmware)
{
  addOpenTxFrskyOptions(firmware);
  addOpenTxFontOptions(firmware);
}

void addPPMInternalModuleHack(OpenTxFirmware * firmware)
{
  firmware->addOption("internalppm", Firmware::tr("Support for PPM internal module hack"));
}

void addOpenTxArm9xOptions(OpenTxFirmware * firmware, bool dblkeys = true)
{
  addOpenTxCommonOptions(firmware);
  firmware->addOption("heli", Firmware::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("gvars", Firmware::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("potscroll", Firmware::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", Firmware::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", Firmware::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("nographics", Firmware::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", Firmware::tr("Battery graph"));
  firmware->addOption("nobold", Firmware::tr("Don't use bold font for highlighting active items"));
  //firmware->addOption("bluetooth", Firmware::tr("Bluetooth interface"));
  if (dblkeys)
    firmware->addOption("dblkeys", Firmware::tr("Enable resetting values by pressing up and down at the same time"));
  addOpenTxFontOptions(firmware);
  addOpenTxRfOptions(firmware, true);
}

void registerOpenTxFirmwares()
{
  OpenTxFirmware * firmware;

  /* FrSky Taranis X9D+ board */
  firmware = new OpenTxFirmware("opentx-x9d+", Firmware::tr("FrSky Taranis X9D+"), BOARD_TARANIS_X9DP);
  firmware->addOption("noras", Firmware::tr("Disable RAS (SWR)"));
  addOpenTxTaranisOptions(firmware);
  addPPMInternalModuleHack(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky Taranis X9D board */
  firmware = new OpenTxFirmware("opentx-x9d", Firmware::tr("FrSky Taranis X9D"), BOARD_TARANIS_X9D);
  firmware->addOption("haptic", Firmware::tr("Haptic module installed"));
  addOpenTxTaranisOptions(firmware);
  addPPMInternalModuleHack(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky Taranis X9E board */
  firmware = new OpenTxFirmware("opentx-x9e", Firmware::tr("FrSky Taranis X9E"), BOARD_TARANIS_X9E);
  firmware->addOption("shutdownconfirm", Firmware::tr("Confirmation before radio shutdown"));
  firmware->addOption("horussticks", Firmware::tr("Horus gimbals installed (Hall sensors)"));
  addOpenTxTaranisOptions(firmware);
  addPPMInternalModuleHack(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X9-Lite board */
  firmware = new OpenTxFirmware("opentx-x9lite", Firmware::tr("FrSky Taranis X9-Lite"), BOARD_TARANIS_X9LITE);
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X7 board */
  firmware = new OpenTxFirmware("opentx-x7", Firmware::tr("FrSky Taranis X7 / X7S"), BOARD_TARANIS_X7);
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X-Lite S/PRO board */
  firmware = new OpenTxFirmware("opentx-xlites", Firmware::tr("FrSky Taranis X-Lite S/PRO"), BOARD_TARANIS_XLITES);
  addOpenTxTaranisOptions(firmware);
  firmware->addOption("internalpxx1", Firmware::tr("Support for PXX1 internal module replacement"));
  registerOpenTxFirmware(firmware);

  /* FrSky X-Lite board */
  firmware = new OpenTxFirmware("opentx-xlite", Firmware::tr("FrSky Taranis X-Lite"), BOARD_TARANIS_XLITE);
  // firmware->addOption("stdr9m", Firmware::tr("Use JR-sized R9M module"));
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X10 board */
  firmware = new OpenTxFirmware("opentx-x10", Firmware::tr("FrSky Horus X10 / X10S"), BOARD_X10);
  addOpenTxFrskyOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X12 (Horus) board */
  firmware = new OpenTxFirmware("opentx-x12s", Firmware::tr("FrSky Horus X12S"), BOARD_X12S);
  addOpenTxFrskyOptions(firmware);
  firmware->addOption("pcbdev", Firmware::tr("Use ONLY with first DEV pcb version"));
  registerOpenTxFirmware(firmware);

  /* Jumper T12 board */
  firmware = new OpenTxFirmware("opentx-t12", QCoreApplication::translate("Firmware", "Jumper T12"), BOARD_JUMPER_T12);
  addOpenTxCommonOptions(firmware);
  firmware->addOption("noheli", Firmware::tr("Disable HELI menu and cyclic mix support"));
  firmware->addOption("nogvars", Firmware::tr("Disable Global variables"));
  firmware->addOption("lua", Firmware::tr("Enable Lua custom scripts screen"));
  firmware->addOption("luac", Firmware::tr("Enable Lua compiler"));
  firmware->addOption("crossfire", Firmware::tr("Support for Crossfire TX Module"));
  addOpenTxFontOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* 9XR-Pro */
  firmware = new OpenTxFirmware("opentx-9xrpro", Firmware::tr("Turnigy 9XR-PRO"), BOARD_9XRPRO);
  addOpenTxArm9xOptions(firmware, false);
  registerOpenTxFirmware(firmware);

  /* ar9x board */
  firmware = new OpenTxFirmware("opentx-ar9x", Firmware::tr("9X with AR9X board"), BOARD_AR9X);
  addOpenTxArm9xOptions(firmware);
  //firmware->addOption("rtc", Firmware::tr("Optional RTC added"));
  //firmware->addOption("volume", Firmware::tr("i2c volume control added"));
  registerOpenTxFirmware(firmware);

  /* Sky9x board */
  firmware = new OpenTxFirmware("opentx-sky9x", Firmware::tr("9X with Sky9x board"), BOARD_SKY9X);
  addOpenTxArm9xOptions(firmware);
  registerOpenTxFirmware(firmware);

  // These are kept only for import purposes, marked as deprecated to hide from UI.
  registerOpenTxFirmware(new OpenTxFirmware("opentx-9xr",      Firmware::tr("Turnigy 9XR"),                       BOARD_STOCK),    true);
  registerOpenTxFirmware(new OpenTxFirmware("opentx-9xr128",   Firmware::tr("Turnigy 9XR with m128 chip"),        BOARD_M128),     true);
  registerOpenTxFirmware(new OpenTxFirmware("opentx-9x",       Firmware::tr("9X with stock board"),               BOARD_STOCK),    true);
  registerOpenTxFirmware(new OpenTxFirmware("opentx-9x128",    Firmware::tr("9X with stock board and m128 chip"), BOARD_M128),     true);
  registerOpenTxFirmware(new OpenTxFirmware("opentx-gruvin9x", Firmware::tr("9X with Gruvin9x board"),            BOARD_GRUVIN9X), true);
  registerOpenTxFirmware(new OpenTxFirmware("opentx-mega2560", Firmware::tr("DIY MEGA2560 radio"),                BOARD_MEGA2560), true);

  Firmware::setDefaultVariant(Firmware::getFirmwareForId("opentx-x9d+"));
  Firmware::setCurrentVariant(Firmware::getDefaultVariant());
}

void unregisterOpenTxFirmwares()
{
  foreach (Firmware * f, Firmware::getRegisteredFirmwares()) {
    delete f;
  }
  unregisterEEpromInterfaces();
}

template <class T, class M>
OpenTxEepromInterface * loadFromByteArray(T & dest, const QByteArray & data)
{
  foreach(OpenTxEepromInterface * eepromInterface, opentxEEpromInterfaces) {
    if (eepromInterface->loadFromByteArray<T, M>(dest, data)) {
      return eepromInterface;
    }
  }
  return NULL;
}

template <class T, class M>
bool saveToByteArray(const T & dest, QByteArray & data)
{
  qDebug() << "ICI SAVE";
  Board::Type board = getCurrentBoard();
  foreach(OpenTxEepromInterface * eepromInterface, opentxEEpromInterfaces) {
    if (eepromInterface->getBoard() == board) {
      return eepromInterface->saveToByteArray<T, M>(dest, data);
    }
  }
  return false;
}

OpenTxEepromInterface * loadModelFromByteArray(ModelData & model, const QByteArray & data)
{
  return loadFromByteArray<ModelData, OpenTxModelData>(model, data);
}

OpenTxEepromInterface * loadRadioSettingsFromByteArray(GeneralSettings & settings, const QByteArray & data)
{
  return loadFromByteArray<GeneralSettings, OpenTxGeneralData>(settings, data);
}

bool writeModelToByteArray(const ModelData & model, QByteArray & data)
{
  return saveToByteArray<ModelData, OpenTxModelData>(model, data);
}

bool writeRadioSettingsToByteArray(const GeneralSettings & settings, QByteArray & data)
{
  return saveToByteArray<GeneralSettings, OpenTxGeneralData>(settings, data);
}
