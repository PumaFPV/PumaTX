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

struct FrSkySportSensor {
  const uint16_t firstId;
  const uint16_t lastId;
  const uint8_t subId;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t prec;
};

const FrSkySportSensor sportSensors[] = {
  { RSSI_ID, RSSI_ID, 0, ZSTR_RSSI, UNIT_DB, 0 },
  { ADC1_ID, ADC1_ID, 0, ZSTR_A1, UNIT_VOLTS, 1 },
  { ADC2_ID, ADC2_ID, 0, ZSTR_A2, UNIT_VOLTS, 1 },
  { A3_FIRST_ID, A3_LAST_ID, 0, ZSTR_A3, UNIT_VOLTS, 2 },
  { A4_FIRST_ID, A4_LAST_ID, 0, ZSTR_A4, UNIT_VOLTS, 2 },
  { BATT_ID, BATT_ID, 0, ZSTR_BATT, UNIT_VOLTS, 1 },
  { R9_PWR_ID, R9_PWR_ID, 0, ZSTR_R9PW, UNIT_MILLIWATTS, 0 },
  { T1_FIRST_ID, T1_LAST_ID, 0, ZSTR_TEMP1, UNIT_CELSIUS, 0 },
  { T2_FIRST_ID, T2_LAST_ID, 0, ZSTR_TEMP2, UNIT_CELSIUS, 0 },
  { RPM_FIRST_ID, RPM_LAST_ID, 0, ZSTR_RPM, UNIT_RPMS, 0 },
  { FUEL_FIRST_ID, FUEL_LAST_ID, 0, ZSTR_FUEL, UNIT_PERCENT, 0 },
  { ALT_FIRST_ID, ALT_LAST_ID, 0, ZSTR_ALT, UNIT_METERS, 2 },
  { VARIO_FIRST_ID, VARIO_LAST_ID, 0, ZSTR_VSPD, UNIT_METERS_PER_SECOND, 2 },
  { ACCX_FIRST_ID, ACCX_LAST_ID, 0, ZSTR_ACCX, UNIT_G, 2 },
  { ACCY_FIRST_ID, ACCY_LAST_ID, 0, ZSTR_ACCY, UNIT_G, 2 },
  { ACCZ_FIRST_ID, ACCZ_LAST_ID, 0, ZSTR_ACCZ, UNIT_G, 2 },
  { CURR_FIRST_ID, CURR_LAST_ID, 0, ZSTR_CURR, UNIT_AMPS, 1 },
  { VFAS_FIRST_ID, VFAS_LAST_ID, 0, ZSTR_VFAS, UNIT_VOLTS, 2 },
  { AIR_SPEED_FIRST_ID, AIR_SPEED_LAST_ID, 0, ZSTR_ASPD, UNIT_KTS, 1 },
  { GPS_SPEED_FIRST_ID, GPS_SPEED_LAST_ID, 0, ZSTR_GSPD, UNIT_KTS, 3 },
  { CELLS_FIRST_ID, CELLS_LAST_ID, 0, ZSTR_CELLS, UNIT_CELLS, 2 },
  { GPS_ALT_FIRST_ID, GPS_ALT_LAST_ID, 0, ZSTR_GPSALT, UNIT_METERS, 2 },
  { GPS_TIME_DATE_FIRST_ID, GPS_TIME_DATE_LAST_ID, 0, ZSTR_GPSDATETIME, UNIT_DATETIME, 0 },
  { GPS_LONG_LATI_FIRST_ID, GPS_LONG_LATI_LAST_ID, 0, ZSTR_GPS, UNIT_GPS, 0 },
  { FUEL_QTY_FIRST_ID, FUEL_QTY_LAST_ID, 0, ZSTR_FUEL, UNIT_MILLILITERS, 2 },
  { GPS_COURS_FIRST_ID, GPS_COURS_LAST_ID, 0, ZSTR_HDG, UNIT_DEGREE, 2 },
  { RBOX_BATT1_FIRST_ID, RBOX_BATT1_LAST_ID, 0, ZSTR_BATT1_VOLTAGE, UNIT_VOLTS, 3 },
  { RBOX_BATT2_FIRST_ID, RBOX_BATT2_LAST_ID, 0, ZSTR_BATT2_VOLTAGE, UNIT_VOLTS, 3 },
  { RBOX_BATT1_FIRST_ID, RBOX_BATT1_LAST_ID, 1, ZSTR_BATT1_CURRENT, UNIT_AMPS, 2 },
  { RBOX_BATT2_FIRST_ID, RBOX_BATT2_LAST_ID, 1, ZSTR_BATT2_CURRENT, UNIT_AMPS, 2 },
  { RBOX_CNSP_FIRST_ID, RBOX_CNSP_LAST_ID, 0, ZSTR_BATT1_CONSUMPTION, UNIT_MAH, 0 },
  { RBOX_CNSP_FIRST_ID, RBOX_CNSP_LAST_ID, 1, ZSTR_BATT2_CONSUMPTION, UNIT_MAH, 0 },
  { RBOX_STATE_FIRST_ID, RBOX_STATE_LAST_ID, 0, ZSTR_CHANS_STATE, UNIT_BITFIELD, 0 },
  { RBOX_STATE_FIRST_ID, RBOX_STATE_LAST_ID, 1, ZSTR_RB_STATE, UNIT_BITFIELD, 0 },
  { SD1_FIRST_ID, SD1_LAST_ID, 0, ZSTR_SD1_CHANNEL, UNIT_RAW, 0 },
  { ESC_POWER_FIRST_ID, ESC_POWER_LAST_ID, 0, ZSTR_ESC_VOLTAGE, UNIT_VOLTS, 2 },
  { ESC_POWER_FIRST_ID, ESC_POWER_LAST_ID, 1, ZSTR_ESC_CURRENT, UNIT_AMPS, 2 },
  { ESC_RPM_CONS_FIRST_ID, ESC_RPM_CONS_LAST_ID, 0, ZSTR_ESC_RPM, UNIT_RPMS, 0 },
  { ESC_RPM_CONS_FIRST_ID, ESC_RPM_CONS_LAST_ID, 1, ZSTR_ESC_CONSUMPTION, UNIT_MAH, 0 },
  { ESC_TEMPERATURE_FIRST_ID, ESC_TEMPERATURE_LAST_ID, 0, ZSTR_ESC_TEMP, UNIT_CELSIUS, 0 },
  { GASSUIT_TEMP1_FIRST_ID, GASSUIT_TEMP1_LAST_ID, 0, ZSTR_GASSUIT_TEMP1, UNIT_CELSIUS, 0 },
  { GASSUIT_TEMP2_FIRST_ID, GASSUIT_TEMP2_LAST_ID, 0, ZSTR_GASSUIT_TEMP2, UNIT_CELSIUS, 0 },
  { GASSUIT_SPEED_FIRST_ID, GASSUIT_SPEED_LAST_ID, 0, ZSTR_GASSUIT_RPM, UNIT_RPMS, 0 },
  { GASSUIT_RES_VOL_FIRST_ID, GASSUIT_RES_VOL_LAST_ID, 0, ZSTR_GASSUIT_RES_VOL, UNIT_MILLILITERS, 0 },
  { GASSUIT_RES_PERC_FIRST_ID, GASSUIT_RES_PERC_LAST_ID, 0, ZSTR_GASSUIT_RES_PERC, UNIT_PERCENT, 0 },
  { GASSUIT_FLOW_FIRST_ID, GASSUIT_FLOW_LAST_ID, 0, ZSTR_GASSUIT_FLOW, UNIT_MILLILITERS_PER_MINUTE, 0 },
  { GASSUIT_MAX_FLOW_FIRST_ID, GASSUIT_MAX_FLOW_LAST_ID, 0, ZSTR_GASSUIT_MAX_FLOW, UNIT_MILLILITERS_PER_MINUTE, 0 },
  { GASSUIT_AVG_FLOW_FIRST_ID, GASSUIT_AVG_FLOW_LAST_ID, 0, ZSTR_GASSUIT_AVG_FLOW, UNIT_MILLILITERS_PER_MINUTE, 0 },
  { SBEC_POWER_FIRST_ID, SBEC_POWER_LAST_ID, 0, ZSTR_SBEC_VOLTAGE, UNIT_VOLTS, 2 },
  { SBEC_POWER_FIRST_ID, SBEC_POWER_LAST_ID, 1, ZSTR_SBEC_CURRENT, UNIT_AMPS, 2 },
  { 0, 0, 0, NULL, UNIT_RAW, 0 } // sentinel
};

const FrSkySportSensor * getFrSkySportSensor(uint16_t id, uint8_t subId=0)
{
  const FrSkySportSensor * result = NULL;
  for (const FrSkySportSensor * sensor = sportSensors; sensor->firstId; sensor++) {
    if (id >= sensor->firstId && id <= sensor->lastId && subId == sensor->subId) {
      result = sensor;
      break;
    }
  }
  return result;
}

bool checkSportPacket(const uint8_t * packet)
{
  short crc = 0;
  for (int i=1; i<FRSKY_SPORT_PACKET_SIZE; ++i) {
    crc += packet[i]; // 0-1FE
    crc += crc >> 8;  // 0-1FF
    crc &= 0x00ff;    // 0-FF
  }
  // TRACE("crc: 0x%02x", crc);
  return (crc == 0x00ff);
}

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

uint16_t servosState;
uint16_t rboxState;

void sportProcessTelemetryPacket(uint16_t id, uint8_t subId, uint8_t instance, uint32_t data, TelemetryUnit unit=UNIT_RAW)
{
  const FrSkySportSensor * sensor = getFrSkySportSensor(id, subId);
  uint8_t precision = 0;
  if (sensor) {
    if (unit == UNIT_RAW)
      unit = sensor->unit;
    precision = sensor->prec;
  }
  if (unit == UNIT_CELLS) {
    uint8_t cellsCount = (data & 0xF0) >> 4;
    uint8_t cellIndex = (data & 0x0F);
    uint32_t mask = (cellsCount << 24) + (cellIndex << 16);
    setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, mask + (((data & 0x000FFF00) >> 8) / 5), unit, precision);
    if (cellIndex+1 < cellsCount) {
      mask += (1 << 16);
      setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, mask + (((data & 0xFFF00000) >> 20) / 5), unit, precision);
    }
  }
  else {
    setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, data, unit, precision);
  }
}

void sportProcessTelemetryPacket(const uint8_t * packet)
{
  if (!checkSportPacket(packet)) {
    TRACE("sportProcessTelemetryPacket(): checksum error ");
    DUMP(packet, FRSKY_SPORT_PACKET_SIZE);
    return;
  }

  sportProcessTelemetryPacketWithoutCrc(TELEMETRY_ENDPOINT_SPORT, packet);
}

void sportProcessTelemetryPacketWithoutCrc(uint8_t origin, const uint8_t * packet)
{
  uint8_t physicalId = packet[0] & 0x1F;
  uint8_t primId = packet[1];
  uint16_t dataId = *((uint16_t *)(packet+2));
  uint32_t data = SPORT_DATA_S32(packet);

#if defined(BLUETOOTH)
  if (g_eeGeneral.bluetoothMode == BLUETOOTH_TELEMETRY && bluetooth.state == BLUETOOTH_STATE_CONNECTED) {
    bluetooth.forwardTelemetry(packet);
  }
#endif

  if (primId == DATA_FRAME) {
    uint8_t instance = physicalId + (origin << 5);
    if (dataId == RSSI_ID) {
      telemetryStreaming = TELEMETRY_TIMEOUT10ms; // reset counter only if valid packets are being detected
      data = SPORT_DATA_U8(packet);
      if (g_model.rssiSource) {
        TelemetrySensor * sensor = &g_model.telemetrySensors[g_model.rssiSource - 1];
        if (sensor->isSameInstance(TELEM_PROTO_FRSKY_SPORT, instance)) {
          telemetryData.rssi.set(data);
        }
      }
      else {
        telemetryData.rssi.set(data);
      }
    }
    else if (dataId == R9_PWR_ID) {
      uint32_t r9pwr[] = {100, 200, 500, 1000};
      data = r9pwr[SPORT_DATA_U8(packet) & 0x03];
    }
    else if (dataId == XJT_VERSION_ID) {
      telemetryData.xjtVersion = HUB_DATA_U16(packet);
      if (!isRasValueValid()) {
        telemetryData.setSwr(origin >> 2, 0);
      }
    }
    else if (dataId == RAS_ID) {
      if (isRasValueValid()) {
        telemetryData.setSwr(origin >> 2, SPORT_DATA_U8(packet));
      }
    }

    if (TELEMETRY_STREAMING()/* because when Rx is OFF it happens that some old A1/A2 values are sent from the XJT module*/) {
      if ((dataId >> 8) == 0) {
        // The old FrSky IDs
        processHubPacket(dataId, HUB_DATA_U16(packet));
      }
      else if (!IS_HIDDEN_TELEMETRY_VALUE(dataId)) {
        if (dataId == ADC1_ID || dataId == ADC2_ID || dataId == BATT_ID || dataId == RAS_ID) {
          data = SPORT_DATA_U8(packet);
        }
        if (dataId >= GPS_LONG_LATI_FIRST_ID && dataId <= GPS_LONG_LATI_LAST_ID) {
          int32_t value = (data & 0x3fffffff);
          if (data & (1 << 30))
            value = -value;
          value = (value * 5) / 3; // min/10000 => deg/1000000
          if (data & (1 << 31))
            sportProcessTelemetryPacket(dataId, 0, instance, value, UNIT_GPS_LONGITUDE);
          else
            sportProcessTelemetryPacket(dataId, 0, instance, value, UNIT_GPS_LATITUDE);
        }
        else if (dataId >= RBOX_BATT1_FIRST_ID && dataId <= RBOX_BATT2_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, data & 0xffff);
          sportProcessTelemetryPacket(dataId, 1, instance, data >> 16);
        }
        else if (dataId >= RBOX_CNSP_FIRST_ID && dataId <= RBOX_CNSP_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, data & 0xffff);
          sportProcessTelemetryPacket(dataId, 1, instance, data >> 16);
        }
        else if (dataId >= RBOX_STATE_FIRST_ID && dataId <= RBOX_STATE_LAST_ID) {
          bool static isRB10 = false;
          uint16_t newServosState;

          if (servosState == 0 && (data & 0xff00) == 0xff00) {
            isRB10 = true;
          }
          if (isRB10) {
            newServosState = data & 0x00ff; // 8ch only RB10
          }
          else {
            newServosState = data & 0xffff;
          }
          if (newServosState != 0 && servosState == 0) {
            audioEvent(AU_SERVO_KO);
          }
          uint16_t newRboxState = data >> 16;
          if ((newRboxState & 0x07) && (rboxState & 0x07) == 0) {
            audioEvent(AU_RX_OVERLOAD);
          }
          servosState = newServosState;
          rboxState = newRboxState;
          sportProcessTelemetryPacket(dataId, 0, instance, servosState);
          sportProcessTelemetryPacket(dataId, 1, instance, rboxState);
        }
        else if (dataId >= ESC_POWER_FIRST_ID && dataId <= ESC_POWER_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, data & 0xffff);
          sportProcessTelemetryPacket(dataId, 1, instance, data >> 16);
        }
        else if (dataId >= ESC_RPM_CONS_FIRST_ID && dataId <= ESC_RPM_CONS_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, 100 * (data & 0xffff));
          sportProcessTelemetryPacket(dataId, 1, instance, data >> 16);
        }
        else if (dataId >= ESC_TEMPERATURE_FIRST_ID && dataId <= ESC_TEMPERATURE_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, data & 0x00ff);
        }
        else if (dataId >= SBEC_POWER_FIRST_ID && dataId <= SBEC_POWER_LAST_ID) {
          sportProcessTelemetryPacket(dataId, 0, instance, (data & 0xffff) / 10);
          sportProcessTelemetryPacket(dataId, 1, instance, (data >> 16) / 10);
        }
        else if (dataId >= DIY_STREAM_FIRST_ID && dataId <= DIY_STREAM_LAST_ID) {
#if defined(LUA)
          if (luaInputTelemetryFifo && luaInputTelemetryFifo->hasSpace(sizeof(SportTelemetryPacket))) {
            SportTelemetryPacket luaPacket;
            luaPacket.physicalId = physicalId;
            luaPacket.primId = primId;
            luaPacket.dataId = dataId;
            luaPacket.value = data;
            for (uint8_t i=0; i<sizeof(SportTelemetryPacket); i++) {
              luaInputTelemetryFifo->push(luaPacket.raw[i]);
            }
          }
#endif
        }
        else {
          sportProcessTelemetryPacket(dataId, 0, instance, data);
        }
      }
    }
  }
#if defined(LUA)
  else if (primId == 0x32) {
    if (luaInputTelemetryFifo && luaInputTelemetryFifo->hasSpace(sizeof(SportTelemetryPacket))) {
      SportTelemetryPacket luaPacket;
      luaPacket.physicalId = physicalId;
      luaPacket.primId = primId;
      luaPacket.dataId = dataId;
      luaPacket.value = data;
      for (uint8_t i=0; i<sizeof(SportTelemetryPacket); i++) {
        luaInputTelemetryFifo->push(luaPacket.raw[i]);
      }
    }
  }
#endif
}

void frskySportSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const FrSkySportSensor * sensor = getFrSkySportSensor(id, subId);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->prec);
    telemetrySensor.init(sensor->name, unit, prec);
    if (id >= ADC1_ID && id <= BATT_ID) {
      telemetrySensor.custom.ratio = 132;
      telemetrySensor.filter = 1;
    }
    else if (id >= CURR_FIRST_ID && id <= CURR_LAST_ID) {
      telemetrySensor.onlyPositive = 1;
    }
    else if (id >= ALT_FIRST_ID && id <= ALT_LAST_ID) {
      telemetrySensor.autoOffset = 1;
    }
    if (unit == UNIT_RPMS) {
      telemetrySensor.custom.ratio = 1;
      telemetrySensor.custom.offset = 1;
    }
    else if (unit == UNIT_METERS) {
      if (IS_IMPERIAL_ENABLE()) {
        telemetrySensor.unit = UNIT_FEET;
      }
    }
    else if (unit == UNIT_GPS_LATITUDE || unit == UNIT_GPS_LONGITUDE) {
      telemetrySensor.unit = UNIT_GPS;
    }
  }
  else {
    telemetrySensor.init(id);
  }

  storageDirty(EE_MODEL);
}
