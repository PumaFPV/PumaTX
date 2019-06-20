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

#define STATUS_BAR_Y     (7*FH+1)
#define TELEM_2ND_COLUMN (10*FW)

uint8_t s_frsky_view = 0;

#define BAR_LEFT    25
#define BAR_WIDTH   100

void displayRssiLine()
{
  if (TELEMETRY_STREAMING()) {
    lcdDrawSolidHorizontalLine(0, 55, 128, 0); // separator
    uint8_t rssi;
    rssi = min((uint8_t)99, TELEMETRY_RSSI());
    lcdDrawNumber(LCD_W/2 -2, STATUS_BAR_Y, rssi, LEADING0 | RIGHT | SMLSIZE, 2);
    lcdDrawText(lcdLastLeftPos,STATUS_BAR_Y, "RSSI : ", RIGHT | SMLSIZE);
    lcdDrawRect(65, 57, 38, 7);
    uint8_t v = 4*rssi/11;
    lcdDrawFilledRect(66+36-v, 58, v, 5, (rssi < g_model.rssiAlarms.getWarningRssi()) ? DOTTED : SOLID);
  }
  else {
    lcdDrawText(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
    lcdInvertLastLine();
  }
}



uint8_t barCoord(int16_t value, int16_t min, int16_t max)
{
  if (value <= min)
    return 0;
  else if (value >= max)
    return BAR_WIDTH-1;
  else
    return ((int32_t)(BAR_WIDTH-1) * (value - min)) / (max - min);
}


bool displayGaugesTelemetryScreen(TelemetryScreenData & screen)
{
  // Custom Screen with gauges
  uint8_t barHeight = 5;
  for (int8_t i=3; i>=0; i--) {
    FrSkyBarData & bar = screen.bars[i];
    source_t source = bar.source;
    getvalue_t barMin = bar.barMin;
    getvalue_t barMax = bar.barMax;
    if (source <= MIXSRC_LAST_CH) {
      barMin = calc100toRESX(barMin);
      barMax = calc100toRESX(barMax);
    }
    if (source && barMax > barMin) {
      uint8_t y = barHeight+6+i*(barHeight+6);
      drawSource(0, y+barHeight/2-3, source, SMLSIZE);
      lcdDrawRect(BAR_LEFT, y, BAR_WIDTH+1, barHeight+2);
      getvalue_t value = getValue(source);

      uint8_t thresholdX = 0;


      uint8_t width = barCoord(value, barMin, barMax);

      uint8_t barShade = SOLID;

      lcdDrawFilledRect(BAR_LEFT+1, y+1, width, barHeight, barShade);

      for (uint8_t j=24; j<99; j+=25) {
        if (j>thresholdX || j>width) {
          lcdDrawSolidVerticalLine(j*BAR_WIDTH/100+BAR_LEFT+1, y+1, barHeight);
        }
      }

      if (thresholdX) {
        lcdDrawVerticalLine(BAR_LEFT+1+thresholdX, y-2, barHeight+3, DOTTED);
        lcdDrawSolidHorizontalLine(BAR_LEFT+thresholdX, y-2, 3);
      }
    }
    else {
      barHeight += 2;
    }
  }
  displayRssiLine();
  return barHeight < 13;
}

bool displayNumbersTelemetryScreen(TelemetryScreenData & screen)
{
  // Custom Screen with numbers
  uint8_t fields_count = 0;
  lcdDrawSolidVerticalLine(63, 8, 48);
  for (uint8_t i=0; i<4; i++) {
    for (uint8_t j=0; j<NUM_LINE_ITEMS; j++) {
      source_t field = screen.lines[i].sources[j];
      if (field > 0) {
        fields_count++;
      }
      if (i==3) {
        if (!TELEMETRY_STREAMING()) {
          displayRssiLine();
          return fields_count;
        }
      }
      if (field) {
        LcdFlags att = (i==3 ? RIGHT|NO_UNIT : RIGHT|MIDSIZE|NO_UNIT);
        coord_t pos[] = {0, 65, 130};
        if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER && i!=3) {
          // there is not enough space on LCD for displaying "Tmr1" or "Tmr2" and still see the - sign, we write "T1" or "T2" instead
          drawStringWithIndex(pos[j], 1+FH+2*FH*i, "T", field-MIXSRC_FIRST_TIMER+1, 0);
        }
        else if (field >= MIXSRC_FIRST_TELEM && isGPSSensor(1+(field-MIXSRC_FIRST_TELEM)/3) && telemetryItems[(field-MIXSRC_FIRST_TELEM)/3].isAvailable()) {
          // we don't display GPS name, no space for it
          att = RIGHT|DBLSIZE|NO_UNIT;  //DBLSIZE ensure the telem screen specific display for GPS is used
        }
        else {
          drawSource(pos[j], 1+FH+2*FH*i, field, 0);
        }

        if (field >= MIXSRC_FIRST_TELEM) {
          TelemetryItem & telemetryItem = telemetryItems[(field-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
          if (!telemetryItem.isAvailable()) {
            continue;
          }
          else if (telemetryItem.isOld()) {
            att |= INVERS|BLINK;
          }
        }
        if(isSensorUnit(1+(field-MIXSRC_FIRST_TELEM)/3, UNIT_DATETIME) && field >= MIXSRC_FIRST_TELEM) {
          drawTelemScreenDate(pos[j+1]-36, 6+FH+2*FH*i, field, SMLSIZE|NO_UNIT);
        }
        else {
          drawSourceValue(pos[j+1]-2, (i==3 ? 1+FH+2*FH*i:FH+2*FH*i), field, att);
        }
      }
    }
  }
  lcdInvertLastLine();
  return fields_count;
}

bool displayCustomTelemetryScreen(uint8_t index)
{
  TelemetryScreenData & screen = g_model.screens[index];

  if (IS_BARS_SCREEN(s_frsky_view)) {
    return displayGaugesTelemetryScreen(screen);
  }

  displayNumbersTelemetryScreen(screen);

  return true;
}

bool displayTelemetryScreen()
{
#if defined(LUA)
  if (TELEMETRY_SCREEN_TYPE(s_frsky_view) == TELEMETRY_SCREEN_TYPE_SCRIPT) {
    uint8_t state = isTelemetryScriptAvailable(s_frsky_view);
    switch (state) {
      case SCRIPT_OK:
        return true;  // contents will be drawed by Lua Task
      case SCRIPT_NOFILE:
        return false;  // requested lua telemetry screen not available
      case SCRIPT_SYNTAX_ERROR:
      case SCRIPT_PANIC:
      case SCRIPT_KILLED:
        luaError(lsScripts, state, false);
        return true;
    }
    return false;
  }
#endif

  if (TELEMETRY_SCREEN_TYPE(s_frsky_view) == TELEMETRY_SCREEN_TYPE_NONE) {
    return false;
  }

  drawTelemetryTopBar();

  if (s_frsky_view < MAX_TELEMETRY_SCREENS) {
    return displayCustomTelemetryScreen(s_frsky_view);
  }



  return true;
}

enum NavigationDirection {
  none,
  up,
  down
};
#define decrTelemetryScreen() direction = up
#define incrTelemetryScreen() direction = down

#if defined(NAVIGATION_XLITE)
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_LONG(KEY_RIGHT)
#elif defined(RADIO_X7)
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_LONG(KEY_PAGE)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_PAGE)
#else
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_FIRST(KEY_UP)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_FIRST(KEY_DOWN)
#endif

void menuViewTelemetryFrsky(event_t event)
{
  enum NavigationDirection direction = none;

  switch (event) {
    case EVT_KEY_FIRST(KEY_EXIT):
#if defined(LUA)
    case EVT_KEY_LONG(KEY_EXIT):
#endif
      killEvents(event);
      chainMenu(menuMainView);
      break;

    case EVT_KEY_PREVIOUS_VIEW:
#if defined(PCBXLITE)
      if (IS_SHIFT_PRESSED()) {
        decrTelemetryScreen();
      }
#else
      if (IS_KEY_LONG(EVT_KEY_PREVIOUS_VIEW)) {
        killEvents(event);
      }
      decrTelemetryScreen();
#endif
      break;

    case EVT_KEY_NEXT_VIEW:
#if defined(PCBXLITE)
      if (IS_SHIFT_PRESSED()) {
        incrTelemetryScreen();
      }
#else
      incrTelemetryScreen();
#endif
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
      POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
      POPUP_MENU_START(onMainViewMenu);
      break;
  }

  for (int i=0; i<=TELEMETRY_SCREEN_TYPE_MAX; i++) {
    if (direction == up) {
      if (s_frsky_view-- == 0)
        s_frsky_view = TELEMETRY_VIEW_MAX;
    }
    else if (direction == down) {
      if (s_frsky_view++ == TELEMETRY_VIEW_MAX)
        s_frsky_view = 0;
    }
    else {
      direction = down;
    }
    if (displayTelemetryScreen()) {
      return;
    }
  }

  drawTelemetryTopBar();
  lcdDrawText(2*FW, 3*FH, "No Telemetry Screens");
  displayRssiLine();
}

#undef EVT_KEY_PREVIOUS_VIEW
#undef EVT_KEY_NEXT_VIEW
