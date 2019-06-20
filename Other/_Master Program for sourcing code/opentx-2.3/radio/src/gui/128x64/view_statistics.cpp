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

#define STATS_1ST_COLUMN               1
#define STATS_2ND_COLUMN               7*FW+FW/2
#define STATS_3RD_COLUMN               14*FW+FW/2
#define STATS_LABEL_WIDTH              3*FW

void menuStatisticsView(event_t event)
{
  title(STR_MENUSTAT);

  switch (event) {
    case EVT_KEY_FIRST(KEY_UP):
#if defined(NAVIGATION_X7)
    case EVT_KEY_BREAK(KEY_PAGE):
#endif
      chainMenu(menuStatisticsDebug);
      break;

    case EVT_KEY_FIRST(KEY_DOWN):
#if defined(NAVIGATION_X7)
    case EVT_KEY_LONG(KEY_PAGE):
      killEvents(event);
      chainMenu(menuStatisticsDebug2);
#else
      chainMenu(menuStatisticsDebug);
#endif
      break;

#if !defined(PCBTARANIS)
    case EVT_KEY_LONG(KEY_MENU): // historical
#endif
#if !defined(PCBSKY9X)
    case EVT_KEY_LONG(KEY_ENTER):
#endif
      g_eeGeneral.globalTimer = 0;
      storageDirty(EE_GENERAL);
      sessionTimer = 0;
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  // Session and Total timers
  lcdDrawText(STATS_1ST_COLUMN, FH*1+1, "SES", BOLD);
  drawTimer(STATS_1ST_COLUMN + STATS_LABEL_WIDTH, FH*1+1, sessionTimer, 0, 0);
  lcdDrawText(STATS_1ST_COLUMN, FH*2+1, "TOT", BOLD);
  drawTimer(STATS_1ST_COLUMN + STATS_LABEL_WIDTH, FH*2+1, g_eeGeneral.globalTimer + sessionTimer, TIMEHOUR, 0);

  // Throttle special timers
  lcdDrawText(STATS_2ND_COLUMN, FH*0+1, "THR", BOLD);
  drawTimer(STATS_2ND_COLUMN + STATS_LABEL_WIDTH, FH*0+1, s_timeCumThr, 0, 0);
  lcdDrawText(STATS_2ND_COLUMN, FH*1+1, "TH%", BOLD);
  drawTimer(STATS_2ND_COLUMN + STATS_LABEL_WIDTH, FH*1+1, s_timeCum16ThrP/16, 0, 0);

  // Timers
  for (int i=0; i<TIMERS; i++) {
    drawStringWithIndex(STATS_3RD_COLUMN, FH*i+1, "TM", i+1, BOLD);
    if (timersStates[i].val > 3600)
      drawTimer(STATS_3RD_COLUMN + STATS_LABEL_WIDTH, FH*i+1, timersStates[i].val, TIMEHOUR, 0);
    else
      drawTimer(STATS_3RD_COLUMN + STATS_LABEL_WIDTH, FH*i+1, timersStates[i].val, 0, 0);
  }

#if defined(THRTRACE)
  const coord_t x = 5;
  const coord_t y = 60;
  lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
  lcdDrawSolidVerticalLine(x, y-32, 32+3);
  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
  }

  uint16_t traceRd = s_traceWr > MAXTRACE ? s_traceWr - MAXTRACE : 0;
  for (coord_t i=1; i<=MAXTRACE && traceRd<s_traceWr; i++, traceRd++) {
    uint8_t h = s_traceBuf[traceRd % MAXTRACE];
    lcdDrawSolidVerticalLine(x+i, y-h, h);
  }
#endif
}

#define MENU_DEBUG_COL1_OFS          (11*FW-3)
#define MENU_DEBUG_COL2_OFS          (17*FW)

void menuStatisticsDebug(event_t event)
{
  title(STR_MENUDEBUG);

  switch (event) {
    case EVT_KEY_LONG(KEY_ENTER):
#if defined(PCBSKY9X)
      g_eeGeneral.mAhUsed = 0;
      Current_used = 0;
#endif
      g_eeGeneral.globalTimer = 0;
      sessionTimer = 0;
      storageDirty(EE_GENERAL);
      killEvents(event);
      break;

    case EVT_KEY_FIRST(KEY_ENTER):
      maxMixerDuration  = 0;
      break;

    case EVT_KEY_FIRST(KEY_UP):
#if defined(NAVIGATION_X7)
    case EVT_KEY_BREAK(KEY_PAGE):
      chainMenu(menuStatisticsDebug2);
      return;
#endif

    case EVT_KEY_FIRST(KEY_DOWN):
#if defined(NAVIGATION_X7)
    case EVT_KEY_LONG(KEY_PAGE):
#endif
      killEvents(event);
      chainMenu(menuStatisticsView);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

#if defined(PCBSKY9X)
  if ((ResetReason&RSTC_SR_RSTTYP) == (2<<8)) {
    lcdDrawText(LCD_W-8*FW, 0, "WATCHDOG");
  }
  else if (unexpectedShutdown) {
    lcdDrawText(LCD_W-13*FW, 0, "UNEXP.SHTDOWN");
  }
#endif

  uint8_t y = FH + 1;

#if defined(TX_CAPACITY_MEASUREMENT)
  // current
  lcdDrawTextAlignedLeft(y, STR_CPU_CURRENT);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, y, getCurrent(), UNIT_MILLIAMPS, LEFT);
  uint32_t current_scale = 488 + g_eeGeneral.txCurrentCalibration;
  lcdDrawChar(MENU_DEBUG_COL2_OFS, y, '>');
  drawValueWithUnit(MENU_DEBUG_COL2_OFS+FW+1, y, Current_max*10*current_scale/8192, UNIT_RAW, LEFT);
  y += FH;

  // consumption
  lcdDrawTextAlignedLeft(y, STR_CPU_MAH);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, y, g_eeGeneral.mAhUsed + Current_used*current_scale/8192/36, UNIT_MAH, LEFT|PREC1);
  y += FH;
#endif

#if defined(PCBSKY9X)
  lcdDrawTextAlignedLeft(y, STR_CPU_TEMP);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, y, getTemperature(), UNIT_TEMPERATURE, LEFT);
  lcdDrawChar(MENU_DEBUG_COL2_OFS, y, '>');
  drawValueWithUnit(MENU_DEBUG_COL2_OFS+FW+1, y, maxTemperature+g_eeGeneral.temperatureCalib, UNIT_TEMPERATURE, LEFT);
  y += FH;
#endif

#if defined(COPROCESSOR)
  lcdDrawTextAlignedLeft(y, STR_COPROC_TEMP);
  if (Coproc_read==0) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, y, "Co Proc NACK",INVERS);
  }
  else if (Coproc_read==0x81) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, y, "Inst.TinyApp",INVERS);
  }
  else if (Coproc_read<3) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, y, "Upgr.TinyApp",INVERS);
  }
  else {
    drawValueWithUnit(MENU_DEBUG_COL1_OFS, y, Coproc_temp, UNIT_TEMPERATURE, LEFT);
    drawValueWithUnit(MENU_DEBUG_COL2_OFS, y, Coproc_maxtemp, UNIT_TEMPERATURE, LEFT);
  }
  y += FH;
#endif

#if defined(STM32) && !defined(SIMU) && defined(DEBUG)
  lcdDrawTextAlignedLeft(y, "Usb");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, charsWritten, LEFT);
  lcdDrawText(lcdLastRightPos, y, " ");
  lcdDrawNumber(lcdLastRightPos, y, APP_Rx_ptr_in, LEFT);
  lcdDrawText(lcdLastRightPos, y, " ");
  lcdDrawNumber(lcdLastRightPos, y, APP_Rx_ptr_out, LEFT);
  lcdDrawText(lcdLastRightPos, y, " ");
  lcdDrawNumber(lcdLastRightPos, y, usbWraps, LEFT);
  y += FH;
#endif

#if defined(STM32)
  lcdDrawTextAlignedLeft(y, "Free Mem");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, availableMemory(), LEFT);
  lcdDrawText(lcdLastRightPos, y, "b");
  y += FH;
#endif

#if defined(LUA)
  lcdDrawTextAlignedLeft(y, "Lua scripts");
  lcdDrawText(MENU_DEBUG_COL1_OFS, y+1, "[D]", SMLSIZE);
  lcdDrawNumber(lcdLastRightPos, y, 10*maxLuaDuration, LEFT);
  lcdDrawText(lcdLastRightPos+2, y+1, "[I]", SMLSIZE);
  lcdDrawNumber(lcdLastRightPos, y, 10*maxLuaInterval, LEFT);
  y += FH;
#endif

  lcdDrawTextAlignedLeft(y, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT);
  lcdDrawText(lcdLastRightPos, y, "ms");
  y += FH;

  lcdDrawTextAlignedLeft(y, STR_FREE_STACK);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, menusStack.available(), LEFT);
  lcdDrawText(lcdLastRightPos, y, "/");
  lcdDrawNumber(lcdLastRightPos, y, mixerStack.available(), LEFT);
  lcdDrawText(lcdLastRightPos, y, "/");
  lcdDrawNumber(lcdLastRightPos, y, audioStack.available(), LEFT);
  y += FH;

#if defined(DEBUG_LATENCY)
  lcdDrawTextAlignedLeft(y, "Hearbeat");
  if (heartbeatCapture.valid)
    lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, heartbeatCapture.count, LEFT);
  else
    lcdDrawText(MENU_DEBUG_COL1_OFS, y, "---");
  y += FH;
#endif

  lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
  lcdInvertLastLine();
}

#if defined(STM32)
void menuStatisticsDebug2(event_t event)
{
  title(STR_MENUDEBUG);

  switch (event) {
    case EVT_KEY_FIRST(KEY_ENTER):
      telemetryErrors  = 0;
      break;

    case EVT_KEY_FIRST(KEY_UP):
#if defined(NAVIGATION_X7)
    case EVT_KEY_BREAK(KEY_PAGE):
#endif
      chainMenu(menuStatisticsView);
      return;

    case EVT_KEY_FIRST(KEY_DOWN):
#if defined(NAVIGATION_X7)
    case EVT_KEY_LONG(KEY_PAGE):
#endif
      killEvents(event);
      chainMenu(menuStatisticsDebug);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  uint8_t y = FH + 1;

  lcdDrawTextAlignedLeft(y, "Tlm RX Err");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, telemetryErrors, RIGHT);
  y += FH;

#if defined(BLUETOOTH)
  lcdDrawTextAlignedLeft(y, "BT status");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, y, IS_BLUETOOTH_CHIP_PRESENT(), RIGHT);
  y += FH;
#endif

  lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
  lcdInvertLastLine();
}
#endif
