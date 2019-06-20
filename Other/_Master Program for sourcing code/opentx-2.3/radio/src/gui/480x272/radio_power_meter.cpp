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

extern uint8_t g_moduleIdx;

enum PowerMeterFields {
  POWER_METER_FREQ_RANGE,
  POWER_METER_ATTENUATOR,
  POWER_METER_MAX,
  POWER_METER_POWER,
  POWER_METER_PEAK,
  POWER_METER_FIELDS_MAX
};

#define POWER_METER_MARGIN_2ND_COLUMN   180

bool menuRadioPowerMeter(event_t event)
{
  SIMPLE_SUBMENU(STR_MENU_POWER_METER, ICON_RADIO, POWER_METER_FIELDS_MAX-1);

  if (TELEMETRY_STREAMING()) {
    lcdDrawCenteredText(LCD_H/2, STR_TURN_OFF_RECEIVER);
    if (event == EVT_KEY_FIRST(KEY_EXIT)) {
      killEvents(event);
      popMenu();
    }
    return true;
  }

  if (menuEvent) {
    lcdDrawCenteredText(LCD_H/2, STR_STOPPING);
    lcdRefresh();
    moduleState[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    /* wait 1s to resume normal operation before leaving */
    watchdogSuspend(1000);
    RTOS_WAIT_MS(1000);
    return true;
  }

  if (moduleState[g_moduleIdx].mode != MODULE_MODE_POWER_METER) {
    memclear(&reusableBuffer.powerMeter, sizeof(reusableBuffer.powerMeter));
    reusableBuffer.powerMeter.freq = 2400000000;
    reusableBuffer.powerMeter.attn = 4;
    moduleState[g_moduleIdx].mode = MODULE_MODE_POWER_METER;
  }

  // The warning
  if (reusableBuffer.powerMeter.attn != 0)
    lcdDrawCenteredText(MENU_CONTENT_TOP, STR_POWERMETER_ATTN_NEEDED, BOLD);

  for (uint8_t i=0; i<POWER_METER_FIELDS_MAX; i++) {
    LcdFlags attr = (menuVerticalPosition == i ? (s_editMode > 0 ? INVERS | BLINK : INVERS) : 0);
    coord_t y = MENU_CONTENT_TOP + 30 + i * FH;

    switch (i) {
      case POWER_METER_FREQ_RANGE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POWERMETER_FREQ);
        lcdDrawNumber(POWER_METER_MARGIN_2ND_COLUMN, y, reusableBuffer.powerMeter.freq / 1000000, LEFT | attr);
        lcdDrawText(lcdNextPos, y, " MHz band");
        if (attr) {
          reusableBuffer.powerMeter.freq = checkIncDec(event, reusableBuffer.powerMeter.freq == 900000000, 0, 1) ? 900000000 : 2400000000;
          if (checkIncDec_Ret) {
            reusableBuffer.powerMeter.power = 0;
            reusableBuffer.powerMeter.peak = 0;
          }
        }
        break;

      case POWER_METER_ATTENUATOR:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POWERMETER_ATTN);
        lcdDrawNumber(POWER_METER_MARGIN_2ND_COLUMN, y, -10 * reusableBuffer.powerMeter.attn, LEFT | attr);
        lcdDrawText(lcdNextPos, y, " dB");
        if (attr) {
          reusableBuffer.powerMeter.attn = checkIncDec(event, reusableBuffer.powerMeter.attn, 0, 5, 0);
        }
        break;

      case POWER_METER_MAX:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MAX);
        lcdDrawNumber(POWER_METER_MARGIN_2ND_COLUMN, y, -10 + 10 * reusableBuffer.powerMeter.attn);
        lcdDrawText(lcdNextPos, y, "dBm/ ");
        drawPower(lcdNextPos, y, -10 + 10 * reusableBuffer.powerMeter.attn, 0);
        if (attr) {
          reusableBuffer.powerMeter.attn = checkIncDec(event, reusableBuffer.powerMeter.attn, 0, 5, 0);
        }
        break;

      case POWER_METER_POWER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POWERMETER_POWER);
        if (reusableBuffer.powerMeter.power) {
          lcdDrawNumber(POWER_METER_MARGIN_2ND_COLUMN, y, reusableBuffer.powerMeter.power + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;

      case POWER_METER_PEAK:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POWERMETER_PEAK);
        if (reusableBuffer.powerMeter.peak) {
          lcdDrawNumber(POWER_METER_MARGIN_2ND_COLUMN, y, reusableBuffer.powerMeter.peak + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;
    }
  }
  return true;
}
