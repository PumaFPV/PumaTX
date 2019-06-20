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

uint8_t g_moduleIdx;

uint8_t getSwitchWarningsCount()
{
  uint8_t count = 0;
  for (int i=0; i<NUM_SWITCHES; ++i) {
    if (SWITCH_WARNING_ALLOWED(i)) {
      ++count;
    }
  }
  return count;
}

enum MenuModelSetupItems {
  ITEM_MODEL_SETUP_NAME,
  ITEM_MODEL_SETUP_BITMAP,
  ITEM_MODEL_SETUP_TIMER1,
  ITEM_MODEL_SETUP_TIMER1_NAME,
  ITEM_MODEL_SETUP_TIMER1_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER1_COUNTDOWN_BEEP,
#if TIMERS > 1
  ITEM_MODEL_SETUP_TIMER2,
  ITEM_MODEL_SETUP_TIMER2_NAME,
  ITEM_MODEL_SETUP_TIMER2_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER2_COUNTDOWN_BEEP,
#endif
#if TIMERS > 2
  ITEM_MODEL_SETUP_TIMER3,
  ITEM_MODEL_SETUP_TIMER3_NAME,
  ITEM_MODEL_SETUP_TIMER3_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER3_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER3_COUNTDOWN_BEEP,
#endif
#if defined(PCBX9E)
  ITEM_MODEL_SETUP_TOP_LCD_TIMER,
#endif
  ITEM_MODEL_SETUP_EXTENDED_LIMITS,
  ITEM_MODEL_SETUP_EXTENDED_TRIMS,
  ITEM_MODEL_SETUP_DISPLAY_TRIMS,
  ITEM_MODEL_SETUP_TRIM_INC,
  ITEM_MODEL_SETUP_THROTTLE_LABEL,
  ITEM_MODEL_SETUP_THROTTLE_REVERSED,
  ITEM_MODEL_SETUP_THROTTLE_TRACE,
  ITEM_MODEL_SETUP_THROTTLE_TRIM,
  ITEM_MODEL_SETUP_PREFLIGHT_LABEL,
  ITEM_MODEL_SETUP_CHECKLIST_DISPLAY,
  ITEM_MODEL_SETUP_THROTTLE_WARNING,
  ITEM_MODEL_SETUP_SWITCHES_WARNING1,
#if defined(PCBX9E)
  ITEM_MODEL_SETUP_SWITCHES_WARNING2,
  ITEM_MODEL_SETUP_SWITCHES_WARNING3,
#endif
  ITEM_MODEL_SETUP_POTS_WARNING,
#if defined(PCBX9E)
  ITEM_MODEL_SETUP_POTS_WARNING2,
#endif
  ITEM_MODEL_SETUP_BEEP_CENTER,
  ITEM_MODEL_SETUP_USE_GLOBAL_FUNCTIONS,
#if defined(PXX2)
  ITEM_MODEL_SETUP_REGISTRATION_ID,
#endif
  ITEM_MODEL_SETUP_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_BIND,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_MODEL_NUM,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_3,

  ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_TYPE,
#if defined (MULTIMODULE)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_BIND,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_MODEL_NUM,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AUTOBIND,
#endif
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_POWER,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_3,

  ITEM_MODEL_SETUP_TRAINER_LABEL,
  ITEM_MODEL_SETUP_TRAINER_MODE,
  ITEM_MODEL_SETUP_TRAINER_LINE1,
  ITEM_MODEL_SETUP_TRAINER_LINE2,
  ITEM_MODEL_SETUP_LINES_COUNT
};

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-17*FW-MENUS_SCROLLBAR_WIDTH-1)
#define MODEL_SETUP_3RD_COLUMN        (MODEL_SETUP_2ND_COLUMN+6*FW)
#define MODEL_SETUP_BIND_OFS          3*FW-2
#define MODEL_SETUP_RANGE_OFS         7*FW
#define MODEL_SETUP_SET_FAILSAFE_OFS  10*FW-2

void copySelection(char * dst, const char * src, uint8_t size)
{
  if (memcmp(src, "---", 3) == 0)
    memset(dst, 0, size);
  else
    memcpy(dst, src, size);
}

void onModelSetupBitmapMenu(const char * result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else if (result != STR_EXIT) {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
  }
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  drawStringWithIndex(0*FW, y, STR_TIMER, timerIdx+1);
  drawTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer.mode, menuHorizontalPosition==0 ? attr : 0);
  drawTimer(MODEL_SETUP_3RD_COLUMN, y, timer.start, menuHorizontalPosition==1 ? attr|TIMEHOUR : TIMEHOUR, menuHorizontalPosition==2 ? attr|TIMEHOUR : TIMEHOUR);
  if (attr && menuHorizontalPosition < 0) {
    lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 13*FW-3, FH+1);
  }
  if (attr && s_editMode>0) {
    div_t qr = div(timer.start, 60);
    switch (menuHorizontalPosition) {
      case 0:
      {
        swsrc_t timerMode = timer.mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer.mode = timerMode;
#if defined(AUTOSWITCH)
        if (s_editMode>0) {
          swsrc_t val = timer.mode - (TMRMODE_COUNT-1);
          swsrc_t switchVal = checkIncDecMovedSwitch(val);
          if (val != switchVal) {
            timer.mode = switchVal + (TMRMODE_COUNT-1);
            storageDirty(EE_MODEL);
          }
        }
#endif
        break;
      }
      case 1:
        qr.quot = checkIncDec(event, qr.quot, 0, 1439, EE_MODEL | NO_INCDEC_MARKS); // 23h59
        timer.start = qr.rem + qr.quot*60;
        break;
      case 2:
        qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
        timer.start -= qr.rem ;
        if ((int16_t)timer.start < 0) timer.start=0;
        if ((int32_t)timer.start > 86399) timer.start=86399; // 23h59:59
        break;
    }
  }
}

void editTimerCountdown(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  lcdDrawTextAlignedLeft(y, STR_BEEPCOUNTDOWN);
  lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VBEEPCOUNTDOWN, timer.countdownBeep, (menuHorizontalPosition==0 ? attr : 0));
  if (timer.countdownBeep != COUNTDOWN_SILENT) {
    lcdDrawNumber(MODEL_SETUP_3RD_COLUMN, y, TIMER_COUNTDOWN_START(timerIdx), (menuHorizontalPosition == 1 ? attr : 0) | LEFT);
    lcdDrawChar(lcdLastRightPos, y, 's');
  }
  if (attr && s_editMode>0) {
    switch (menuHorizontalPosition) {
      case 0:
        CHECK_INCDEC_MODELVAR(event, timer.countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT - 1);
        break;
      case 1:
        timer.countdownStart = -checkIncDecModel(event, -timer.countdownStart, -1, +2);
        break;
    }
  }
}

#define IF_INTERNAL_MODULE_ON(x)          (IS_INTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)
#define IF_EXTERNAL_MODULE_ON(x)          (IS_EXTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)

#if defined(INTERNAL_MODULE_PXX1)
#define INTERNAL_MODULE_TYPE_ROWS         ((isModuleXJT(INTERNAL_MODULE) || isModulePXX2(INTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0) // Module type + RF protocols
#else
#define INTERNAL_MODULE_TYPE_ROWS         (0) // Module type + RF protocols
#endif

#define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : 1))

#define TRAINER_LINE1_ROWS                (g_model.trainerData.mode == TRAINER_MODE_SLAVE ? (uint8_t)1 : HIDDEN_ROW)
#define TRAINER_LINE2_ROWS                (g_model.trainerData.mode == TRAINER_MODE_SLAVE ? (uint8_t)2 : HIDDEN_ROW)

#define TIMER_ROWS(x)                     2|NAVIGATION_LINE_BY_LINE, 0, 0, 0, g_model.timers[x].countdownBeep != COUNTDOWN_SILENT ? (uint8_t) 1 : (uint8_t)0

#define EXTERNAL_MODULE_MODE_ROWS         (isModuleXJT(EXTERNAL_MODULE) || isModuleR9M(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0

#if TIMERS == 1
  #define TIMERS_ROWS                     TIMER_ROWS(0)
#elif TIMERS == 2
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1)
#elif TIMERS == 3
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1), TIMER_ROWS(2)
#endif
#if defined(PCBX9E)
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), uint8_t(getSwitchWarningsCount() > 8 ? TITLE_ROW : HIDDEN_ROW), uint8_t(getSwitchWarningsCount() > 16 ? TITLE_ROW : HIDDEN_ROW)
  #define POT_WARN_ROWS                  uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|(NUM_POTS+NUM_SLIDERS) : 0), uint8_t(g_model.potsWarnMode ? TITLE_ROW : HIDDEN_ROW)
  #define TOPLCD_ROWS                     0,
#else
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsCount())
  #define POT_WARN_ROWS                  uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|(NUM_POTS+NUM_SLIDERS) : 0)
  #define TOPLCD_ROWS
#endif

#define IF_PXX2_MODULE(module, xxx)          (isModulePXX2(module) ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_PXX2_MODULE(module, xxx)      (isModulePXX2(module) ? HIDDEN_ROW : (uint8_t)(xxx))
#define IF_ACCESS_MODULE_RF(module, xxx)     (isModuleRFAccess(module) ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_ACCESS_MODULE_RF(module, xxx) (isModuleRFAccess(module) ? HIDDEN_ROW : (uint8_t)(xxx))

#if defined(PXX2)
#define REGISTRATION_ID_ROWS          uint8_t((isDefaultModelRegistrationID() || (warningText && popupFunc == runPopupRegister)) ? HIDDEN_ROW : READONLY_ROW),
#else
#define REGISTRATION_ID_ROWS
#endif

#define CURRENT_MODULE_EDITED(k)      (k >= ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE)
#define CURRENT_RECEIVER_EDITED(k)    (k - (k >= ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL ? ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1 : ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1))

#include "common/stdlcd/model_setup_pxx1.cpp"

#if defined(PXX2)
#include "common/stdlcd/model_setup_pxx2.cpp"
#endif

void menuModelSetup(event_t event)
{
  horzpos_t l_posHorz = menuHorizontalPosition;
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);

  int8_t old_editMode = s_editMode;

  MENU_TAB({
    HEADER_LINE_COLUMNS

    0, // Model name
    0, // Model image

    TIMERS_ROWS,

    TOPLCD_ROWS

    0, // Extended limits
    1, // Extended trims
    0, // Show trims
    0, // Trims step

    LABEL(Throttle),
      0, // Thrxottle reverse
      0, // Throttle trace source
      0, // Throttle trim

    LABEL(PreflightCheck),
      0, // Checklist
      0, // Throttle warning
      SW_WARN_ROWS, // Switch warning
      POT_WARN_ROWS, // Pot warning

    NAVIGATION_LINE_BY_LINE | (NUM_STICKS+NUM_POTS+NUM_SLIDERS-1), // Center beeps

    0, // Global functions

    REGISTRATION_ID_ROWS

    LABEL(InternalModule),
      INTERNAL_MODULE_TYPE_ROWS,
      INTERNAL_MODULE_CHANNELS_ROWS,
      IF_NOT_ACCESS_MODULE_RF(INTERNAL_MODULE, IF_INTERNAL_MODULE_ON(HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1)),
      IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0), // RxNum
      IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)), // Failsafe
      IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 1), // Range check and Register buttons
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),      // Module options
      IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0), // Receiver 1
      IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0), // Receiver 2
      IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0), // Receiver 3

    LABEL(ExternalModule),
      EXTERNAL_MODULE_MODE_ROWS,
      MULTIMODULE_STATUS_ROWS
      EXTERNAL_MODULE_CHANNELS_ROWS,
      IF_NOT_ACCESS_MODULE_RF(EXTERNAL_MODULE, ((isModuleXJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) || isModuleSBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (isModulePPM(EXTERNAL_MODULE) || isModulePXX(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW),
      IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // RxNum
      IF_NOT_PXX2_MODULE(EXTERNAL_MODULE, EXTERNAL_MODULE_OPTION_ROW),
      MULTIMODULE_MODULE_ROWS
      EXTERNAL_MODULE_POWER_ROW,
      FAILSAFE_ROWS(EXTERNAL_MODULE),
      IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 1),   // Range check and Register buttons
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),        // Module options
      IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 1
      IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 2
      IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 3

    LABEL(Trainer),
      0,
      TRAINER_LINE1_ROWS,
      TRAINER_LINE2_ROWS
  });

  MENU_CHECK(menuTabModel, MENU_MODEL_SETUP, ITEM_MODEL_SETUP_LINES_COUNT);
  title(STR_MENUSETUP);

  if (event == EVT_ENTRY) {
    reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
  }

#if (defined(DSM2) || defined(PXX))
  if (menuEvent) {
    moduleState[0].mode = 0;
    moduleState[1].mode = 0;
  }
#endif

  int sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_SETUP_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.header.name, sizeof(g_model.header.name), event, attr);
        memcpy(modelHeaders[g_eeGeneral.currModel].name, g_model.header.name, sizeof(g_model.header.name));
        break;

      case ITEM_MODEL_SETUP_BITMAP:
        lcdDrawTextAlignedLeft(y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), g_model.header.bitmap, LIST_NONE_SD_FILE)) {
            POPUP_MENU_START(onModelSetupBitmapMenu);
          }
          else {
            POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
          }
        }
        break;

      case ITEM_MODEL_SETUP_TIMER1:
        editTimerMode(0, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, INDENT TR_NAME, g_model.timers[0].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER1_MINUTE_BEEP:
        g_model.timers[0].minuteBeep = editCheckBox(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, INDENT TR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_COUNTDOWN_BEEP:
        editTimerCountdown(0, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_PERSISTENT:
        g_model.timers[0].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_SETUP_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, INDENT TR_NAME, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER2_MINUTE_BEEP:
        g_model.timers[1].minuteBeep = editCheckBox(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, INDENT TR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_COUNTDOWN_BEEP:
        editTimerCountdown(1, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_PERSISTENT:
        g_model.timers[1].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
        break;
#endif

#if TIMERS > 2
      case ITEM_MODEL_SETUP_TIMER3:
        editTimerMode(2, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, INDENT TR_NAME, g_model.timers[2].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER3_MINUTE_BEEP:
        g_model.timers[2].minuteBeep = editCheckBox(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, INDENT TR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_COUNTDOWN_BEEP:
        editTimerCountdown(2, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_PERSISTENT:
        g_model.timers[2].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

#if defined(PCBX9E)
      case ITEM_MODEL_SETUP_TOP_LCD_TIMER:
        lcdDrawTextAlignedLeft(y, STR_TOPLCDTIMER);
        drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER, g_model.toplcdTimer+1, attr);
        if (attr) {
          g_model.toplcdTimer = checkIncDec(event, g_model.toplcdTimer, 0, TIMERS-1, EE_MODEL);
        }
        break;
#endif

      case ITEM_MODEL_SETUP_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_SETUP_EXTENDED_TRIMS:
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
        if (attr && menuHorizontalPosition>0) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            START_NO_HIGHLIGHT();
            for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
              memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
            }
            storageDirty(EE_MODEL);
            AUDIO_WARNING1();
          }
        }
        break;

      case ITEM_MODEL_SETUP_DISPLAY_TRIMS:
        g_model.displayTrims = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_DISPLAY_TRIMS, STR_VDISPLAYTRIMS, g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_SETUP_TRIM_INC:
        g_model.trimInc = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_LABEL:
        lcdDrawTextAlignedLeft(y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_REVERSED:
        ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
        break;

      case ITEM_MODEL_SETUP_THROTTLE_TRACE:
      {
        lcdDrawTextAlignedLeft(y, STR_TTRACE);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, g_model.thrTraceSrc, NUM_POTS+NUM_SLIDERS+MAX_OUTPUT_CHANNELS, isThrottleSourceAvailable);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS+NUM_SLIDERS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS - NUM_SLIDERS;
        drawSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_SETUP_THROTTLE_TRIM:
        ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
        break;

      case ITEM_MODEL_SETUP_PREFLIGHT_LABEL:
        lcdDrawTextAlignedLeft(y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_SETUP_CHECKLIST_DISPLAY:
        ON_OFF_MENU_ITEM(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, STR_CHECKLIST, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

#if defined(PCBX9E)
      case ITEM_MODEL_SETUP_SWITCHES_WARNING2:
      case ITEM_MODEL_SETUP_SWITCHES_WARNING3:
      case ITEM_MODEL_SETUP_POTS_WARNING2:
        if (i==0) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
        }
        break;
#endif

      case ITEM_MODEL_SETUP_SWITCHES_WARNING1:
      {
#if defined(PCBX9E)
        if (i>=NUM_BODY_LINES-2 && getSwitchWarningsCount() > 8*(NUM_BODY_LINES-i)) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif
        lcdDrawTextAlignedLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              case EVT_KEY_BREAK(KEY_ENTER):
                break;

              case EVT_KEY_LONG(KEY_ENTER):
                if (menuHorizontalPosition < 0) {
                  START_NO_HIGHLIGHT();
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                killEvents(event);
                break;
            }
          }
        }

        LcdFlags line = attr;

        int current = 0;
        for (int i=0; i<NUM_SWITCHES; i++) {
          if (SWITCH_WARNING_ALLOWED(i)) {
            div_t qr = div(current, 8);
            if (!READ_ONLY() && event==EVT_KEY_BREAK(KEY_ENTER) && line && l_posHorz==current) {
              g_model.switchWarningEnable ^= (1 << i);
              storageDirty(EE_MODEL);
            }
            uint8_t swactive = !(g_model.switchWarningEnable & (1<<i));
            c = "\300-\301"[states & 0x03];
            lcdDrawChar(MODEL_SETUP_2ND_COLUMN+qr.rem*(2*FW+1), y+FH*qr.quot, 'A'+i, line && (menuHorizontalPosition==current) ? INVERS : 0);
            if (swactive) lcdDrawChar(lcdNextPos, y+FH*qr.quot, c);
            ++current;
          }
          states >>= 2;
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(PCBX9E)
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), 1+FH*((current+7)/8));
#else
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, current*(2*FW+1), FH+1);
#endif
        }
        break;
      }

      case ITEM_MODEL_SETUP_POTS_WARNING:
#if defined(PCBX9E)
        if (i==NUM_BODY_LINES-1 && g_model.potsWarnMode) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif

        lcdDrawTextAlignedLeft(y, STR_POTWARNING);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, "\004""OFF\0""Man\0""Auto", g_model.potsWarnMode, (menuHorizontalPosition == 0) ? attr : 0);
        if (attr && (menuHorizontalPosition == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }

        if (attr) {
          if (menuHorizontalPosition > 0) s_editMode = 0;
          if (!READ_ONLY() && menuHorizontalPosition > 0) {
            switch (event) {
              case EVT_KEY_LONG(KEY_ENTER):
                killEvents(event);
                if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
                  SAVE_POT_POSITION(menuHorizontalPosition-1);
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition-1));
                storageDirty(EE_MODEL);
                break;
            }
          }
        }
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+28;
          for (int i=0; i<NUM_POTS+NUM_SLIDERS; ++i) {
            if (i<NUM_XPOTS && !IS_POT_SLIDER_AVAILABLE(POT1+i)) {
              if (attr && (menuHorizontalPosition==i+1)) REPEAT_LAST_CURSOR_MOVE();
            }
            else {
#if defined(PCBX9E)
              if (i == NUM_XPOTS) {
                y += FH;
                x = MODEL_SETUP_2ND_COLUMN;
              }
#endif
              LcdFlags flags = ((menuHorizontalPosition==i+1) && attr) ? BLINK : 0;
              if ((!attr || menuHorizontalPosition >= 0) && !(g_model.potsWarnEnabled & (1 << i))) {
                flags |= INVERS;
              }

              // TODO add a new function
              lcdDrawSizedText(x, y, STR_VSRCRAW+2+STR_VSRCRAW[0]*(NUM_STICKS+1+i), STR_VSRCRAW[0]-1, flags & ~ZCHAR);
              x = lcdNextPos+3;
            }
          }
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(PCBX9E)
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-FH-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, 2*FH+1);
#else
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, FH+1);
#endif
        }
        break;

      case ITEM_MODEL_SETUP_BEEP_CENTER:
      {
        lcdDrawTextAlignedLeft(y, STR_BEEPCTR);
        coord_t x = MODEL_SETUP_2ND_COLUMN;
        for (int i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++) {
          if (i>=POT1 && i<POT1+NUM_XPOTS && !IS_POT_SLIDER_AVAILABLE(i)) {
            if (attr && menuHorizontalPosition == i) REPEAT_LAST_CURSOR_MOVE();
            continue;
          }
          lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
          x += FW;
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
              storageDirty(EE_MODEL);
            }
          }
        }
        break;
      }

      case ITEM_MODEL_SETUP_USE_GLOBAL_FUNCTIONS:
        lcdDrawTextAlignedLeft(y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, STR_INTERNALRF);
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE:
      {
        lcdDrawTextAlignedLeft(y, INDENT TR_MODE);
#if defined(INTERNAL_MODULE_PXX1)
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_INTERNAL_MODULE_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(INTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + 3, y, STR_ACCST_RF_PROTOCOLS, 1+g_model.moduleData[INTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleXJT2(INTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + 3, y, STR_ISRM_PXX2_RF_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].subType, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (menuHorizontalPosition == 0) {
            uint8_t moduleType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL, isInternalModuleAvailable);
            if (checkIncDec_Ret) {
              // TODO this code should be common, in module.h (X10_new_UI branch)
              memclear(&g_model.moduleData[INTERNAL_MODULE], sizeof(ModuleData));
              g_model.moduleData[INTERNAL_MODULE].type = moduleType;
              g_model.moduleData[INTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
          else if (isModuleXJT(INTERNAL_MODULE)) {
            g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, 0, MODULE_SUBTYPE_PXX1_LAST, EE_MODEL, isRfProtocolAvailable);
            if (checkIncDec_Ret) {
              g_model.moduleData[0].type = MODULE_TYPE_XJT_PXX1;
              g_model.moduleData[0].channelsStart = 0;
              g_model.moduleData[0].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
          else if (isModulePXX2(INTERNAL_MODULE)) {
            g_model.moduleData[INTERNAL_MODULE].subType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].subType, 0, MODULE_SUBTYPE_ISRM_PXX2_LAST, EE_MODEL, isRfProtocolAvailable);
          }
        }
#else
        uint8_t index = 0;
        if (g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_ISRM_PXX2) {
          index = 1 + g_model.moduleData[INTERNAL_MODULE].subType;
        }
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_ISRM_PXX2_RF_PROTOCOLS, index, attr);
        if (attr) {
          index = checkIncDec(event, index, 0, 3, EE_MODEL);
          if (checkIncDec_Ret) {
            memclear(&g_model.moduleData[INTERNAL_MODULE], sizeof(ModuleData));
            if (index > 0) {
              g_model.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_ISRM_PXX2;
              g_model.moduleData[INTERNAL_MODULE].subType = index - 1;
              g_model.moduleData[INTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
        }
#endif
        break;
      }

      case ITEM_MODEL_SETUP_TRAINER_MODE:
        lcdDrawTextAlignedLeft(y, INDENT TR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerData.mode, attr);
        if (attr) {
          g_model.trainerData.mode = checkIncDec(event, g_model.trainerData.mode, 0, TRAINER_MODE_MAX(), EE_MODEL, isTrainerModeAvailable);
        }
        break;

      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, STR_EXTERNALRF);
        break;

      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_TYPE:
        lcdDrawTextAlignedLeft(y, INDENT TR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_EXTERNAL_MODULE_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + 3, y, STR_ACCST_RF_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleDSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + 3, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleR9M(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + 3, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].subType, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(EXTERNAL_MODULE)) {
          uint8_t multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);

          // Do not use MODEL_SETUP_3RD_COLUMN here since some the protocol string are so long that we cannot afford the 2 spaces (+6) here
          if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition == 1 ? attr : 0);
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 14 * FW, y, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), menuHorizontalPosition == 2 ? attr : 0, 2);
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 16 * FW, y, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition == 3 ? attr : 0, 2);
          }
          else {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition==1 ? attr : 0);
            const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
            if (pdef->subTypeString != nullptr) {
              lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+11*FW, y, pdef->subTypeString, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
            }
          }
        }
#endif
        if (attr) {
          if (s_editMode > 0) {
            switch (menuHorizontalPosition) {
              case 0:
                g_model.moduleData[EXTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type, MODULE_TYPE_NONE,
                                                                       IS_TRAINER_EXTERNAL_MODULE() ? MODULE_TYPE_NONE : MODULE_TYPE_COUNT - 1, EE_MODEL,
                                                                       isExternalModuleAvailable);
                if (checkIncDec_Ret) {
                  g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                  g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(EXTERNAL_MODULE);
                  if (isModuleSBUS(EXTERNAL_MODULE))
                    g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate = -31;
                  if (isModulePPM(EXTERNAL_MODULE))
                    SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
                }
                break;
              case 1:
                if (isModuleDSM2(EXTERNAL_MODULE))
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
#if defined(MULTIMODULE)
                  else if (isModuleMultimodule(EXTERNAL_MODULE)) {
                    int multiRfProto = g_model.moduleData[EXTERNAL_MODULE].multi.customProto == 1 ? MODULE_SUBTYPE_MULTI_CUSTOM : g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
                    CHECK_INCDEC_MODELVAR(event, multiRfProto, MODULE_SUBTYPE_MULTI_FIRST, MODULE_SUBTYPE_MULTI_LAST);
                    if (checkIncDec_Ret) {
                      g_model.moduleData[EXTERNAL_MODULE].multi.customProto = (multiRfProto == MODULE_SUBTYPE_MULTI_CUSTOM);
                      if (!g_model.moduleData[EXTERNAL_MODULE].multi.customProto)
                        g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(multiRfProto);
                      g_model.moduleData[EXTERNAL_MODULE].subType = 0;
                      // Sensible default for DSM2 (same as for ppm): 7ch@22ms + Autodetect settings enabled
                      if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_DSM2) {
                        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 1;
                      }
                      else {
                        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 0;
                      }
                      g_model.moduleData[EXTERNAL_MODULE].multi.optionValue = 0;
                    }
                  }
#endif
                else if (isModuleR9M(EXTERNAL_MODULE)) {
                  g_model.moduleData[EXTERNAL_MODULE].subType = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].subType,
                                                                            MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_LAST, EE_MODEL,
                                                                            isR9MModeAvailable);
                }
                else {
                  g_model.moduleData[EXTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol,
                                                                               MODULE_SUBTYPE_PXX1_ACCST_D16, MODULE_SUBTYPE_PXX1_LAST, EE_MODEL,
                                                                               isRfProtocolAvailable);
                }
                if (checkIncDec_Ret) {
                  g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(EXTERNAL_MODULE);
                }
                break;

#if defined(MULTIMODULE)
              case 2:
                if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
                  g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), 0, 63, EE_MODEL));
                  break;
                }
                else {
                  const mm_protocol_definition * pdef = getMultiProtocolDefinition(g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false));
                  if (pdef->maxSubtype > 0)
                    CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, pdef->maxSubtype);
                }
                break;

              case 3:
                // Custom protocol, third column is subtype
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 7);
                break;
#endif
            }
          }
          else if (old_editMode > 0) {
            if (isModuleR9M(EXTERNAL_MODULE)) {
              if (g_model.moduleData[EXTERNAL_MODULE].subType > MODULE_SUBTYPE_R9M_EU) {
                POPUP_WARNING(STR_R9M_PROTO_FLEX_WARN_LINE1);
                SET_WARNING_INFO(STR_R9M_PROTO_WARN_LINE2, sizeof(TR_R9M_PROTO_WARN_LINE2) - 1, 0);
              }
              else if (g_model.moduleData[EXTERNAL_MODULE].subType == MODULE_SUBTYPE_R9M_EU) {
                POPUP_WARNING(STR_R9M_PROTO_EU_WARN_LINE1);
                SET_WARNING_INFO(STR_R9M_PROTO_WARN_LINE2, sizeof(TR_R9M_PROTO_WARN_LINE2) - 1, 0);
              }
              else {
                POPUP_WARNING(STR_R9M_PROTO_FCC_WARN_LINE1);
                SET_WARNING_INFO(STR_R9M_PROTO_WARN_LINE2, sizeof(TR_R9M_PROTO_WARN_LINE2) - 1, 0);
              }
            }
          }
        }
        break;

      case ITEM_MODEL_SETUP_TRAINER_LABEL:
        lcdDrawTextAlignedLeft(y, STR_TRAINER);
        break;

      case ITEM_MODEL_SETUP_TRAINER_LINE1:
        lcdDrawTextAlignedLeft(y, STR_CHANNELRANGE);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
        lcdDrawNumber(lcdLastRightPos, y, g_model.trainerData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
        lcdDrawChar(lcdLastRightPos, y, '-');
        lcdDrawNumber(lcdLastRightPos + FW+1, y, g_model.trainerData.channelsStart+8+g_model.trainerData.channelsCount, LEFT | (menuHorizontalPosition==1 ? attr : 0));
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.trainerData.channelsStart, 32-8-g_model.trainerData.channelsCount);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.channelsCount, -4, min<int8_t>(MAX_TRAINER_CHANNELS_M8, 32-8-g_model.trainerData.channelsStart));
              break;
          }
        }
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawNumber(lcdLastRightPos, y, moduleData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
          lcdDrawChar(lcdLastRightPos, y, '-');
          lcdDrawNumber(lcdLastRightPos + FW+1, y, moduleData.channelsStart+sentModuleChannels(moduleIdx), LEFT | (menuHorizontalPosition==1 ? attr : 0));
          if (IS_R9M_OR_XJTD16(moduleIdx)) {
            if (sentModuleChannels(moduleIdx) > 8)
              lcdDrawText(lcdLastRightPos+5, y, "(18ms)");
            else
              lcdDrawText(lcdLastRightPos+5, y, "(9ms)");
          }
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(maxModuleChannels_M8(moduleIdx), 32-8-moduleData.channelsStart));
#if defined(INTERNAL_MODULE_PPM)
                if ((k == ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS && g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_SETUP_TRAINER_LINE1)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
#else
                if ((k == ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_SETUP_TRAINER_LINE1)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
#endif
                break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_SETUP_TRAINER_LINE2:
        lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.trainerData.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
        lcdDrawText(lcdLastRightPos, y, STR_MS);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+6*FW, y, (g_model.trainerData.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
        lcdDrawChar(lcdLastRightPos, y, 'u');
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+12*FW, y, g_model.trainerData.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.frameLength, -20, 35);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.delay, -4, 10);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.trainerData.pulsePol, 1);
              break;
          }
        }
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_BIND:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_BIND:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (isModulePPM(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawText(lcdLastRightPos, y, STR_MS);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+6*FW, y, (moduleData.ppm.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
          lcdDrawChar(lcdLastRightPos, y, 'u');
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+12*FW, y, moduleData.ppm.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);

          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.delay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.ppm.pulsePol, 1);
                break;
            }
          }
        }
        else if (isModuleSBUS(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_REFRESHRATE);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawText(lcdLastRightPos, y, STR_MS);
          lcdDrawText(MODEL_SETUP_3RD_COLUMN, y, moduleData.sbus.noninverted ? "not inverted" : "normal", (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);

          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -33, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.sbus.noninverted, 1);
                break;
            }
          }
        }
        else {
          horzpos_t l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (isModuleXJT(moduleIdx) && IS_D8_RX(MODULE_SUBTYPE_PXX1_ACCST_D8)) {
            xOffsetBind = 0;
            lcdDrawTextAlignedLeft(y, STR_RECEIVER);
            if (attr) l_posHorz += 1;
          }
          else {
            lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
          }
          if (isModulePXX2(moduleIdx) || isModulePXX(moduleIdx) || isModuleDSM2(moduleIdx) || isModuleMultimodule(moduleIdx)) {
            if (xOffsetBind)
              lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
                if (checkIncDec_Ret) {
                  modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
                }
                else if (event == EVT_KEY_LONG(KEY_ENTER)) {
                  killEvents(event);
                  uint8_t newVal = findNextUnusedModelId(g_eeGeneral.currModel, moduleIdx);
                  if (newVal != g_model.header.modelId[moduleIdx]) {
                    modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx] = newVal;
                    storageDirty(EE_MODEL);
                  }
                }
              }
            }
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);
            uint8_t newFlag = 0;
#if defined(MULTIMODULE)
            if (multiBindStatus == MULTI_BIND_FINISHED) {
              multiBindStatus = MULTI_NORMAL_OPERATION;
              s_editMode = 0;
            }
#endif
            if (attr && l_posHorz>0) {
              if (s_editMode>0) {
                if (l_posHorz == 1) {
                  if (isModuleR9M(moduleIdx) || (isModuleXJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol == MODULE_SUBTYPE_PXX1_ACCST_D16)) {
                    if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                      uint8_t default_selection = 0; // R9M_LBT should default to 0 as available options are variables
                      if (isModuleR9M_LBT(moduleIdx)) {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx) && BIND_CH9TO16_ALLOWED(moduleIdx))
                         POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        if (BIND_CH9TO16_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                      }
                      else {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                        default_selection = g_model.moduleData[moduleIdx].pxx.receiver_telem_off + (g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 << 1);
                      }
                      POPUP_MENU_SELECT_ITEM(default_selection);
                      POPUP_MENU_START(onBindMenu);
                      continue;
                    }
                    if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
                      newFlag = MODULE_MODE_BIND;
                    }
                    else {
                      if (!popupMenuItemsCount) {
                        s_editMode = 0;  // this is when popup is exited before a choice is made
                      }
                    }
                  }
                  else {
                    newFlag = MODULE_MODE_BIND;
                  }
                }
                else if (l_posHorz == 2) {
                  newFlag = MODULE_MODE_RANGECHECK;
                }
              }
            }
            moduleState[moduleIdx].mode = newFlag;
#if defined(MULTIMODULE)
            if (newFlag == MODULE_MODE_BIND)
              multiBindStatus = MULTI_BIND_INITIATED;
#endif
          }
        }
        break;
      }

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_FAILSAFE:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_FAILSAFE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_FAILSAFE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition==0 ? attr : 0);
        if (moduleData.failsafeMode == FAILSAFE_CUSTOM) lcdDrawText(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (moduleData.failsafeMode != FAILSAFE_CUSTOM) {
            menuHorizontalPosition = 0;
          }
          if (menuHorizontalPosition == 0) {
            if (s_editMode > 0) {
              CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
              if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
            }
          }
          else if (menuHorizontalPosition == 1) {
            s_editMode = 0;
            if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
              if (event == EVT_KEY_LONG(KEY_ENTER)) {
                killEvents(event);
                setCustomFailsafe(moduleIdx);
                storageDirty(EE_MODEL);
                AUDIO_WARNING1();
                SEND_FAILSAFE_NOW(moduleIdx);
              }
              else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
          }
          else {
            lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W - MODEL_SETUP_2ND_COLUMN - MENUS_SCROLLBAR_WIDTH, 8);
          }
        }
        break;
      }
      break;

     case ITEM_MODEL_SETUP_EXTERNAL_MODULE_OPTIONS:
     {
       uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
#if defined(MULTIMODULE)

       if (isModuleMultimodule(moduleIdx)) {
         int optionValue = g_model.moduleData[moduleIdx].multi.optionValue;

         const uint8_t multi_proto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true);
         const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_proto);
         if (pdef->optionsstr)
           lcdDrawText(INDENT_WIDTH, y, pdef->optionsstr);

         if (multi_proto == MODULE_SUBTYPE_MULTI_FS_AFHDS2A)
           optionValue = 50 + 5 * optionValue;

         lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, optionValue, LEFT | attr);
         if (attr) {
           if (multi_proto == MODULE_SUBTYPE_MULTI_FS_AFHDS2A) {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, 0, 70);
           }
           else if (multi_proto == MODULE_SUBTYPE_MULTI_OLRS) {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -1, 7);
           }
           else {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -128, 127);
           }
         }
       }
#endif
       if (isModuleR9M(moduleIdx)) {
         lcdDrawTextAlignedLeft(y, STR_MODULE_TELEMETRY);
         if (IS_TELEMETRY_INTERNAL_MODULE()) {
           lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_DISABLE_INTERNAL);
         }
         else {
           lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_TELEM_ON);
         }
       }
       else if (isModuleSBUS(moduleIdx)) {
         lcdDrawTextAlignedLeft(y, STR_WARN_BATTVOLTAGE);
         putsVolts(lcdLastRightPos, y, getBatteryVoltage(), attr | PREC2 | LEFT);
       }
     }
     break;

    case  ITEM_MODEL_SETUP_EXTERNAL_MODULE_POWER:
    {
      uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
      if (isModuleR9M(moduleIdx)) {
        lcdDrawTextAlignedLeft(y, TR_MULTI_RFPOWER);
        if (isModuleR9M_FCC_VARIANT(moduleIdx)) {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
          if (attr)
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_FCC_POWER_MAX);
        }
        else {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
          if (attr)
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_LBT_POWER_MAX);
          if (attr && s_editMode == 0 && reusableBuffer.moduleSetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
            if ((reusableBuffer.moduleSetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) // switching between mode 2 and 3 does not require rebind
              POPUP_WARNING(STR_REBIND);
            reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
          }
        }
      }
#if defined (MULTIMODULE)
      else if (isModuleMultimodule(moduleIdx)) {
        g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
      }
#endif
      break;
    }

#if defined (MULTIMODULE)
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AUTOBIND:
      if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_DSM2)
        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
      else
        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_AUTOBIND, attr, event);
      break;

    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_STATUS:
    {
      lcdDrawTextAlignedLeft(y, STR_MODULE_STATUS);
      char statusText[64];
      multiModuleStatus.getStatusString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }

    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_SYNCSTATUS:
    {
      lcdDrawTextAlignedLeft(y, STR_MODULE_SYNC);
      char statusText[64];
      multiSyncStatus.getRefreshString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }
#endif

#if defined(PXX2)
      case ITEM_MODEL_SETUP_REGISTRATION_ID:
        lcdDrawTextAlignedLeft(y, STR_REG_ID);
        if (isDefaultModelRegistrationID())
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_PXX2_DEFAULT);
        else
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, ZCHAR);
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_MODEL_NUM:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_MODEL_NUM:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], attr | LEADING0 | LEFT, 2);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
          if (checkIncDec_Ret) {
            modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
          }
        }
      }
      break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_REGISTER_RANGE:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_REGISTER_RANGE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        lcdDrawTextAlignedLeft(y, STR_MODULE);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON(TR_REGISTER), (menuHorizontalPosition == 0 ? attr : 0));
        lcdDrawText(lcdLastRightPos + 3, y, STR_MODULE_RANGE, (menuHorizontalPosition == 1 ? attr : 0));
        if (attr) {
          if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && s_editMode > 0) {
            if (menuHorizontalPosition == 0 && event == EVT_KEY_BREAK(KEY_ENTER)) {
              startRegisterDialog(moduleIdx);
            }
            else if (menuHorizontalPosition == 1) {
              moduleState[moduleIdx].mode = MODULE_MODE_RANGECHECK;
            }
          }
          if (s_editMode == 0 && !warningText) {
            moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
          }
          if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL) {
            // REGISTER finished
            s_editMode = 0;
          }
        }
      }
      break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_OPTIONS:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_OPTIONS:
        lcdDrawText(INDENT_WIDTH, y, STR_OPTIONS);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_SET, attr);
        if (event == EVT_KEY_BREAK(KEY_ENTER) && attr) {
          g_moduleIdx = CURRENT_MODULE_EDITED(k);
          pushMenu(menuModelModuleOptions);
        }
        break;

      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_3:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_3:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        ModuleInformation & moduleInformation = reusableBuffer.moduleSetup.pxx2.moduleInformation;

        drawStringWithIndex(INDENT_WIDTH, y, STR_RECEIVER, receiverIdx + 1);

        if (!(g_model.moduleData[moduleIdx].pxx2.receivers & (1 << receiverIdx))) {
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_BIND, attr);
          if (attr && s_editMode > 0) {
            s_editMode = 0;
            killEvents(event);
            g_model.moduleData[moduleIdx].pxx2.receivers |= (1 << receiverIdx);
            memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
            storageDirty(EE_MODEL);
          }
          else {
            break;
          }
        }

        drawReceiverName(MODEL_SETUP_2ND_COLUMN, y, moduleIdx, receiverIdx, attr);

        if (s_editMode && isModuleR9MAccess(moduleIdx) && moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && moduleInformation.information.modelID) {
          moduleInformation.information.modelID = 0;
          moduleState[moduleIdx].startBind(&reusableBuffer.moduleSetup.bindInformation);
        }

        if (attr && (moduleState[moduleIdx].mode == 0 || s_editMode == 0)) {
          if (moduleState[moduleIdx].mode) {
            moduleState[moduleIdx].mode = 0;
            removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
            killEvents(event); // we stopped BIND / SHARE, we don't want to re-open the menu
            event = 0;
            CLEAR_POPUP();
          }
          s_editMode = 0;
        }

        if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
          if (reusableBuffer.moduleSetup.bindInformation.step == BIND_INIT) {
            if (reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount > 0) {
              popupMenuItemsCount = min<uint8_t>(reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount, PXX2_MAX_RECEIVERS_PER_MODULE);
              for (uint8_t i = 0; i < popupMenuItemsCount; i++) {
                popupMenuItems[i] = reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[i];
              }
              popupMenuTitle = STR_PXX2_SELECT_RX;
              CLEAR_POPUP();
              POPUP_MENU_START(onPXX2BindMenu);
            }
            else {
              POPUP_WAIT(STR_WAITING_FOR_RX);
            }
          }
        }

        if (attr && EVT_KEY_MASK(event) == KEY_ENTER) {
          killEvents(event);
          if (!isSimu() && isPXX2ReceiverEmpty(moduleIdx, receiverIdx)) {
            onPXX2ReceiverMenu(STR_BIND);
          }
          else {
            POPUP_MENU_ADD_ITEM(STR_BIND);
            POPUP_MENU_ADD_ITEM(STR_OPTIONS);
            POPUP_MENU_ADD_ITEM(STR_SHARE);
            POPUP_MENU_ADD_ITEM(STR_DELETE);
            POPUP_MENU_ADD_ITEM(STR_RESET);
          }
          POPUP_MENU_START(onPXX2ReceiverMenu);
        }
      }
      break;
#endif

    }
  }

#if defined(PXX)
  if (IS_RANGECHECK_ENABLE()) {
    showMessageBox("RSSI: ");
    lcdDrawNumber(WARNING_LINE_X, 5*FH, TELEMETRY_RSSI(), BOLD | LEFT);
  }
#endif

  if (old_editMode > 0 && s_editMode == 0) {
    switch(menuVerticalPosition) {
    case ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_BIND:
      if (menuHorizontalPosition == 0)
        checkModelIdUnique(g_eeGeneral.currModel, INTERNAL_MODULE);
      break;

    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_BIND:
      if (menuHorizontalPosition == 0)
        checkModelIdUnique(g_eeGeneral.currModel, EXTERNAL_MODULE);
      break;
    }
  }
}
