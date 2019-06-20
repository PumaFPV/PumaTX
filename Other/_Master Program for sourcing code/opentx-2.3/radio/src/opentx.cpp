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

RadioData  g_eeGeneral;
ModelData  g_model;

#if defined(SDCARD)
Clipboard clipboard;
#endif

uint8_t unexpectedShutdown = 0;

uint16_t vbattRTC;

/* ARM: mixer duration in 0.5us */
uint16_t maxMixerDuration;

uint8_t heartbeat;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
safetych_t safetyCh[MAX_OUTPUT_CHANNELS];
#endif

// __DMA for the MSC_BOT_Data member
union ReusableBuffer reusableBuffer __DMA;

#if defined(STM32)
uint8_t* MSC_BOT_Data = reusableBuffer.MSC_BOT_Data;
#endif

#if defined(DEBUG_LATENCY)
uint8_t latencyToggleSwitch = 0;
#endif

const uint8_t bchout_ar[]  = {
    0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
    0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
    0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
    0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4 };

uint8_t channel_order(uint8_t setup, uint8_t x)
{
  return ((*(bchout_ar + setup) >> (6 - (x - 1) * 2)) & 3) + 1;
}

uint8_t channel_order(uint8_t x)
{
  return channel_order(g_eeGeneral.templateSetup, x);
}

/*
mode1 rud ele thr ail
mode2 rud thr ele ail
mode3 ail ele thr rud
mode4 ail thr ele rud
*/
const uint8_t modn12x3[]  = {
    0, 1, 2, 3,
    0, 2, 1, 3,
    3, 1, 2, 0,
    3, 2, 1, 0 };

volatile uint8_t rtc_count = 0;
uint32_t watchdogTimeout = 0;

void watchdogSuspend(uint32_t timeout)
{
  watchdogTimeout = timeout;
}

void per10ms()
{
  g_tmr10ms++;

  if (watchdogTimeout) {
    watchdogTimeout -= 1;
    wdt_reset();  // Retrigger hardware watchdog
  }

#if defined(GUI)
  if (lightOffCounter) lightOffCounter--;
  if (flashCounter) flashCounter--;
  if (noHighlightCounter) noHighlightCounter--;
#endif

  if (trimsCheckTimer) trimsCheckTimer--;
  if (ppmInputValidityTimer) ppmInputValidityTimer--;

  if (trimsDisplayTimer)
    trimsDisplayTimer--;
  else
    trimsDisplayMask = 0;

#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    g_rtcTime++;   // inc global unix timestamp one second
#if defined(COPROCESSOR)
    if (g_rtcTime < 60 || rtc_count<5) {
      rtcInit();
      rtc_count++;
    }
    else {
      coprocReadData(true);
    }
#endif
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#if defined(ROTARY_ENCODER_NAVIGATION)
  if (IS_ROTARY_ENCODER_NAVIGATION_ENABLE()) {
    static rotenc_t rePreviousValue;
    static bool cw = false;
    rotenc_t reNewValue = (ROTARY_ENCODER_NAVIGATION_VALUE / ROTARY_ENCODER_GRANULARITY);
    rotenc_t scrollRE = reNewValue - rePreviousValue;
    if (scrollRE) {
      static uint32_t lastEvent;
      rePreviousValue = reNewValue;

      bool new_cw = (scrollRE < 0) ? false : true;
      if ((g_tmr10ms - lastEvent >= 10) || (cw == new_cw)) { // 100ms

        putEvent(new_cw ? EVT_ROTARY_RIGHT : EVT_ROTARY_LEFT);

        // rotary encoder navigation speed (acceleration) detection/calculation
        static uint32_t delay = 2*ROTENC_DELAY_MIDSPEED;

        if (new_cw == cw) {
          // Modified moving average filter used for smoother change of speed
          delay = (((g_tmr10ms - lastEvent) << 3) + delay) >> 1;
        }
        else {
          delay = 2*ROTENC_DELAY_MIDSPEED;
        }

        if (delay < ROTENC_DELAY_HIGHSPEED)
          rotencSpeed = ROTENC_HIGHSPEED;
        else if (delay < ROTENC_DELAY_MIDSPEED)
          rotencSpeed = ROTENC_MIDSPEED;
        else
          rotencSpeed = ROTENC_LOWSPEED;
        cw = new_cw;
        lastEvent = g_tmr10ms;
      }
    }
  }
#endif

#if defined(TELEMETRY_FRSKY) || defined(TELEMETRY_JETI)
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol[0])) {
    telemetryInterrupt10ms();
  }
#endif

  // These moved here from evalFlightModeMixes() to improve beep trigger reliability.
#if defined(PWM_BACKLIGHT)
  if ((g_tmr10ms&0x03) == 0x00)
    backlightFade(); // increment or decrement brightness until target brightness is reached
#endif

#if !defined(AUDIO)
  if (mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING(1);
  if (mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING(2);
  if (mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING(3);
#endif

#if defined(SDCARD)
  sdPoll10ms();
#endif

  outputTelemetryBuffer.per10ms();

  heartbeat |= HEART_TIMER_10MS;
}

FlightModeData *flightModeAddress(uint8_t idx)
{
  return &g_model.flightModeData[idx];
}

ExpoData *expoAddress(uint8_t idx )
{
  return &g_model.expoData[idx];
}

MixData *mixAddress(uint8_t idx)
{
  return &g_model.mixData[idx];
}

LimitData *limitAddress(uint8_t idx)
{
  return &g_model.limitData[idx];
}


void memswap(void * a, void * b, uint8_t size)
{
  uint8_t * x = (uint8_t *)a;
  uint8_t * y = (uint8_t *)b;
  uint8_t temp ;

  while (size--) {
    temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
}

void generalDefault()
{
  memclear(&g_eeGeneral, sizeof(g_eeGeneral));
  g_eeGeneral.version  = EEPROM_VER;
  g_eeGeneral.variant = EEPROM_VARIANT;

#if defined(PCBHORUS)
  g_eeGeneral.blOffBright = 20;
#else
  g_eeGeneral.contrast = LCD_CONTRAST_DEFAULT;
#endif

#if defined(DEFAULT_POTS_CONFIG)
  g_eeGeneral.potsConfig = DEFAULT_POTS_CONFIG;
#endif

#if defined(DEFAULT_SWITCH_CONFIG)
  g_eeGeneral.switchConfig = DEFAULT_SWITCH_CONFIG;
#endif

#if defined(DEFAULT_SLIDERS_CONFIG)
  g_eeGeneral.slidersConfig = DEFAULT_SLIDERS_CONFIG;
#endif

  // vBatWarn is voltage in 100mV, vBatMin is in 100mV but with -9V offset, vBatMax has a -12V offset
  g_eeGeneral.vBatWarn = BATTERY_WARN;
  if (BATTERY_MIN != 90)
    g_eeGeneral.vBatMin = BATTERY_MIN - 90;
  if (BATTERY_MAX != 120)
    g_eeGeneral.vBatMax = BATTERY_MAX - 120;

#if defined(DEFAULT_MODE)
  g_eeGeneral.stickMode = DEFAULT_MODE - 1;
#endif

#if defined(FRSKY_RELEASE)
  g_eeGeneral.templateSetup = 17; /* TAER */
#endif

  g_eeGeneral.backlightMode = e_backlight_mode_all;
  g_eeGeneral.lightAutoOff = 2;
  g_eeGeneral.inactivityTimer = 10;

  g_eeGeneral.ttsLanguage[0] = 'e';
  g_eeGeneral.ttsLanguage[1] = 'n';
  g_eeGeneral.wavVolume = 2;
  g_eeGeneral.backgroundVolume = 1;

  for (int i=0; i<NUM_STICKS; ++i) {
    g_eeGeneral.trainer.mix[i].mode = 2;
    g_eeGeneral.trainer.mix[i].srcChn = channel_order(i+1) - 1;
    g_eeGeneral.trainer.mix[i].studWeight = 100;
  }

#if defined(PCBX9E)
  const int8_t defaultName[] = { 20, -1, -18, -1, -14, -9, -19 };
  memcpy(g_eeGeneral.bluetoothName, defaultName, sizeof(defaultName));
#endif

#if !defined(EEPROM)
  strcpy(g_eeGeneral.currModelFilename, DEFAULT_MODEL_FILENAME);
#endif

#if defined(COLORLCD)
  strcpy(g_eeGeneral.themeName, theme->getName());
  theme->init();
#endif

#if defined(PXX2)
  for (uint8_t i=0; i<PXX2_LEN_REGISTRATION_ID; i++) {
    g_eeGeneral.ownerRegistrationID[i] = (cpu_uid[1 + i] & 0x3f) - 26;
  }
#endif

  g_eeGeneral.chkSum = 0xFFFF;
}

uint16_t evalChkSum()
{
  uint16_t sum = 0;
  const int16_t *calibValues = (const int16_t *) &g_eeGeneral.calib[0];
  for (int i=0; i<12; i++)
    sum += calibValues[i];
  return sum;
}

void clearInputs()
{
  memset(g_model.expoData, 0, sizeof(g_model.expoData)); // clear all expos
}

void defaultInputs()
{
  clearInputs();

  for (int i=0; i<NUM_STICKS; i++) {
    uint8_t stick_index = channel_order(i+1);
    ExpoData *expo = expoAddress(i);
    expo->srcRaw = MIXSRC_Rud - 1 + stick_index;
    expo->curve.type = CURVE_REF_EXPO;
    expo->chn = i;
    expo->weight = 100;
    expo->mode = 3; // TODO constant
#if defined(TRANSLATIONS_CZ)
    for (int c=0; c<4; c++) {
      g_model.inputNames[i][c] = char2zchar(STR_INPUTNAMES[1+4*(stick_index-1)+c]);
    }
#else
    for (int c=0; c<3; c++) {
      g_model.inputNames[i][c] = char2zchar(STR_VSRCRAW[2 + 4 * stick_index + c]);
    }
#if LEN_INPUT_NAME > 3
    g_model.inputNames[i][3] = '\0';
#endif
#endif
  }
  storageDirty(EE_MODEL);
}

void applyDefaultTemplate()
{
  defaultInputs(); // calls storageDirty internally

  for (int i=0; i<NUM_STICKS; i++) {
    MixData * mix = mixAddress(i);
    mix->destCh = i;
    mix->weight = 100;
    mix->srcRaw = i+1;
  }
}

#if defined(EEPROM)
void checkModelIdUnique(uint8_t index, uint8_t module)
{
  if (isModulePXX(module) && IS_D8_RX(module))
    return;

  uint8_t modelId = g_model.header.modelId[module];
  uint8_t additionalOnes = 0;
  char * name = reusableBuffer.moduleSetup.msg;

  memset(reusableBuffer.moduleSetup.msg, 0, sizeof(reusableBuffer.moduleSetup.msg));

  if (modelId != 0) {
    for (uint8_t i = 0; i < MAX_MODELS; i++) {
      if (i != index) {
        if (modelId == modelHeaders[i].modelId[module]) {
          if ((WARNING_LINE_LEN - 4 - (name - reusableBuffer.moduleSetup.msg)) > (signed)(modelHeaders[i].name[0] ? zlen(modelHeaders[i].name, LEN_MODEL_NAME) : sizeof(TR_MODEL) + 2)) { // you cannot rely exactly on WARNING_LINE_LEN so using WARNING_LINE_LEN-2 (-2 for the ",")
            if (reusableBuffer.moduleSetup.msg[0] != '\0') {
              name = strAppend(name, ", ");
            }
            if (modelHeaders[i].name[0] == 0) {
              name = strAppend(name, STR_MODEL);
              name = strAppendUnsigned(name+strlen(name), i + 1, 2);
            }
            else {
              name += zchar2str(name, modelHeaders[i].name, LEN_MODEL_NAME);
            }
          }
          else {
            additionalOnes++;
          }
        }
      }
    }
  }

  if (additionalOnes) {
    name = strAppend(name, " (+");
    name = strAppendUnsigned(name, additionalOnes);
    strAppend(name, ")");
  }

  if (reusableBuffer.moduleSetup.msg[0] != '\0') {
    POPUP_WARNING(STR_MODELIDUSED);
    SET_WARNING_INFO(reusableBuffer.moduleSetup.msg, sizeof(reusableBuffer.moduleSetup.msg), 0);
  }
}

uint8_t findNextUnusedModelId(uint8_t index, uint8_t module)
{
  // assume 63 is the highest Model ID
  // and use 64 bits
  uint8_t usedModelIds[8];
  memset(usedModelIds, 0, sizeof(usedModelIds));

  for (uint8_t mod_i = 0; mod_i < MAX_MODELS; mod_i++) {

    if (mod_i == index)
      continue;

    uint8_t id = modelHeaders[mod_i].modelId[module];
    if (id == 0)
      continue;

    uint8_t mask = 1;
    for (uint8_t i = 1; i < (id & 7); i++)
      mask <<= 1;

    usedModelIds[id >> 3] |= mask;
  }

  uint8_t new_id = 1;
  uint8_t tst_mask = 1;
  for (;new_id < MAX_RX_NUM(module); new_id++) {
    if (!(usedModelIds[new_id >> 3] & tst_mask)) {
      // found free ID
      return new_id;
    }
    if ((tst_mask <<= 1) == 0)
      tst_mask = 1;
  }

  // failed finding something...
  return 0;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));

  applyDefaultTemplate();

  memcpy(g_model.modelRegistrationID, g_eeGeneral.ownerRegistrationID, PXX2_LEN_REGISTRATION_ID);

#if defined(LUA) && defined(PCBTARANIS) //Horus uses menuModelWizard() for wizard
  if (isFileAvailable(WIZARD_PATH "/" WIZARD_NAME)) {
    f_chdir(WIZARD_PATH);
    luaExec(WIZARD_NAME);
  }
#endif

#if defined(HARDWARE_INTERNAL_MODULE)
  g_model.moduleData[INTERNAL_MODULE].type = IS_PXX2_INTERNAL_ENABLED() ? MODULE_TYPE_ISRM_PXX2 : MODULE_TYPE_XJT_PXX1;
  g_model.moduleData[INTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
#elif defined(PCBSKY9X)
  g_model.moduleData[EXTERNAL_MODULE].type = MODULE_TYPE_PPM;
#elif defined(RADIO_T12)
  g_model.moduleData[EXTERNAL_MODULE].type = MODULE_TYPE_NONE;
  g_model.moduleData[EXTERNAL_MODULE].type = MODULE_TYPE_MULTIMODULE;
#endif

#if defined(PCBXLITE)
  g_model.trainerData.mode = TRAINER_MODE_MASTER_BLUETOOTH;
#endif

#if defined(EEPROM)
  for (int i=0; i<NUM_MODULES; i++) {
    modelHeaders[id].modelId[i] = g_model.header.modelId[i] = id+1;
  }
  checkModelIdUnique(id, 0);
#endif

#if defined(FLIGHT_MODES) && defined(GVARS)
  for (int p=1; p<MAX_FLIGHT_MODES; p++) {
    for (int i=0; i<MAX_GVARS; i++) {
      g_model.flightModeData[p].gvars[i] = GVAR_MAX+1;
    }
  }
#endif

#if !defined(EEPROM)
  strcpy(g_model.header.name, "\015\361\374\373\364");
  g_model.header.name[5] = '\033' + id/10;
  g_model.header.name[6] = '\033' + id%10;
#endif

#if defined(PCBHORUS)
  extern const LayoutFactory * defaultLayout;
  delete customScreens[0];
  customScreens[0] = defaultLayout->create(&g_model.screenData[0].layoutData);
  strcpy(g_model.screenData[0].layoutName, "Layout2P1");
  extern const WidgetFactory * defaultWidget;
  customScreens[0]->createWidget(0, defaultWidget);
  // enable switch warnings
  for (int i=0; i<NUM_SWITCHES; i++) {
    g_model.switchWarningState |= (1 << (3*i));
  }
#endif
}

bool isInputRecursive(int index)
{
  ExpoData * line = expoAddress(0);
  for (int i=0; i<MAX_EXPOS; i++, line++) {
    if (line->chn > index)
      break;
    else if (line->chn < index)
      continue;
    else if (line->srcRaw >= MIXSRC_FIRST_LOGICAL_SWITCH)
      return true;
  }
  return false;
}

#if defined(AUTOSOURCE)
int8_t getMovedSource(GET_MOVED_SOURCE_PARAMS)
{
  int8_t result = 0;
  static tmr10ms_t s_move_last_time = 0;

  static int16_t inputsStates[MAX_INPUTS];
  if (min <= MIXSRC_FIRST_INPUT) {
    for (uint8_t i=0; i<MAX_INPUTS; i++) {
      if (abs(anas[i] - inputsStates[i]) > 512) {
        if (!isInputRecursive(i)) {
          result = MIXSRC_FIRST_INPUT+i;
          break;
        }
      }
    }
  }

  static int16_t sourcesStates[NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_MOUSE_ANALOGS];
  if (result == 0) {
    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++) {
      if (abs(calibratedAnalogs[i] - sourcesStates[i]) > 512) {
        result = MIXSRC_Rud+i;
        break;
      }
    }
  }

  bool recent = ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10);
  if (recent) {
    result = 0;
  }

  if (result || recent) {
    memcpy(inputsStates, anas, sizeof(inputsStates));
    memcpy(sourcesStates, calibratedAnalogs, sizeof(sourcesStates));
  }

  s_move_last_time = get_tmr10ms();
  return result;
}
#endif

#if defined(FLIGHT_MODES)
uint8_t getFlightMode()
{
  for (uint8_t i=1; i<MAX_FLIGHT_MODES; i++) {
    FlightModeData *phase = &g_model.flightModeData[i];
    if (phase->swtch && getSwitch(phase->swtch)) {
      return i;
    }
  }
  return 0;
}
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
  FlightModeData * p = flightModeAddress(phase);
  return p->trim[idx];
}

int getTrimValue(uint8_t phase, uint8_t idx)
{
  int result = 0;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    trim_t v = getRawTrimValue(phase, idx);
    if (v.mode == TRIM_MODE_NONE) {
      return result;
    }
    else {
      unsigned int p = v.mode >> 1;
      if (p == phase || phase == 0) {
        return result + v.value;
      }
      else {
        phase = p;
        if (v.mode % 2 != 0) {
          result += v.value;
        }
      }
    }
  }
  return 0;
}

bool setTrimValue(uint8_t phase, uint8_t idx, int trim)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    trim_t & v = flightModeAddress(phase)->trim[idx];
    if (v.mode == TRIM_MODE_NONE)
      return false;
    unsigned int p = v.mode >> 1;
    if (p == phase || phase == 0) {
      v.value = trim;
      break;
    }
    else if (v.mode % 2 == 0) {
      phase = p;
    }
    else {
      v.value = limit<int>(TRIM_EXTENDED_MIN, trim - getTrimValue(p, idx), TRIM_EXTENDED_MAX);
      break;
    }
  }
  storageDirty(EE_MODEL);
  return true;
}

getvalue_t convert16bitsTelemValue(source_t channel, ls_telemetry_value_t value)
{
  return value;
}

#if defined(TELEMETRY_FRSKY)
ls_telemetry_value_t minTelemValue(source_t channel)
{
  return 0;
}

ls_telemetry_value_t maxTelemValue(source_t channel)
{
  return 30000;
}
#endif

#define INAC_STICKS_SHIFT   6
#define INAC_SWITCHES_SHIFT 8
bool inputsMoved()
{
  uint8_t sum = 0;
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++)
    sum += anaIn(i) >> INAC_STICKS_SHIFT;
  for (uint8_t i=0; i<NUM_SWITCHES; i++)
    sum += getValue(MIXSRC_FIRST_SWITCH+i) >> INAC_SWITCHES_SHIFT;
#if defined(GYRO)
  for (uint8_t i=0; i<2; i++)
    sum += getValue(MIXSRC_GYRO1+i) >> INAC_STICKS_SHIFT;
#endif

  if (abs((int8_t)(sum-inactivity.sum)) > 1) {
    inactivity.sum = sum;
    return true;
  }
  else {
    return false;
  }
}

void checkBacklight()
{
  static uint8_t tmr10ms ;

  uint8_t x = g_blinkTmr10ms;
  if (tmr10ms != x) {
    tmr10ms = x;
    if (inputsMoved()) {
      inactivity.counter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks) {
        backlightOn();
      }
    }

    bool backlightOn = (g_eeGeneral.backlightMode == e_backlight_mode_on || (g_eeGeneral.backlightMode != e_backlight_mode_off && lightOffCounter) || isFunctionActive(FUNCTION_BACKLIGHT));
    if (flashCounter) backlightOn = !backlightOn;
    if (backlightOn)
      BACKLIGHT_ENABLE();
    else
      BACKLIGHT_DISABLE();
  }
}

void doLoopCommonActions()
{
  checkBacklight();
}

void backlightOn()
{
  lightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#if MENUS_LOCK == 1
bool readonly = true;
bool readonlyUnlocked()
{
  if (readonly) {
    POPUP_WARNING(STR_MODS_FORBIDDEN);
    return false;
  }
  else {
    return true;
  }
}
#endif

#if defined(SPLASH)
void doSplash()
{
#if defined(PWR_BUTTON_PRESS)
  bool refresh = false;
#endif

  if (SPLASH_NEEDED()) {
    backlightOn();
    drawSplash();

#if defined(PCBSKY9X)
    tmr10ms_t curTime = get_tmr10ms() + 10;
    uint8_t contrast = 10;
    lcdSetRefVolt(contrast);
#endif

    getADC(); // init ADC array

    inputsMoved();

    tmr10ms_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;

    while (tgtime > get_tmr10ms()) {
      RTOS_WAIT_TICKS(1);

      getADC();

      if (keyDown() || inputsMoved()) return;

#if defined(PWR_BUTTON_PRESS)
      uint32_t pwr_check = pwrCheck();
      if (pwr_check == e_power_off) {
        break;
      }
      else if (pwr_check == e_power_press) {
        refresh = true;
      }
      else if (pwr_check == e_power_on && refresh) {
        drawSplash();
        refresh = false;
      }
#else
      if (pwrCheck() == e_power_off) {
        return;
      }
#endif

#if defined(FRSKY_RELEASE)
      static uint8_t secondSplash = false;
      if (!secondSplash && get_tmr10ms() >= tgtime-200) {
        secondSplash = true;
        drawSecondSplash();
      }
#endif

#if defined(PCBSKY9X)
      if (curTime < get_tmr10ms()) {
        curTime += 10;
        if (contrast < g_eeGeneral.contrast) {
          contrast += 1;
          lcdSetRefVolt(contrast);
        }
      }
#endif

      doLoopCommonActions();
    }
  }
}
#else
#define Splash()
#define doSplash()
#endif

#if defined(SDCARD)
void checkSDVersion()
{
  if (sdMounted()) {
    FIL versionFile;
    UINT read = 0;
    char version[sizeof(REQUIRED_SDCARD_VERSION)-1];
    char error[sizeof(TR_WRONG_SDCARDVERSION)+ sizeof(version)];

    strAppend(strAppend(error, STR_WRONG_SDCARDVERSION, sizeof(TR_WRONG_SDCARDVERSION)), REQUIRED_SDCARD_VERSION, sizeof(REQUIRED_SDCARD_VERSION));
    FRESULT result = f_open(&versionFile, "/opentx.sdcard.version", FA_OPEN_EXISTING | FA_READ);
    if (result == FR_OK) {
      if (f_read(&versionFile, &version, sizeof(version), &read) != FR_OK ||
          read != sizeof(version) ||
          strncmp(version, REQUIRED_SDCARD_VERSION, sizeof(version)) != 0) {
        TRACE("SD card version mismatch:  %.*s, %s", sizeof(REQUIRED_SDCARD_VERSION)-1, version, REQUIRED_SDCARD_VERSION);
        ALERT(STR_SD_CARD, error, AU_ERROR);
      }
      f_close(&versionFile);
    }
    else {
      ALERT(STR_SD_CARD, error, AU_ERROR);
    }
  }
}
#endif

#if defined(PCBTARANIS) || defined(PCBHORUS)
void checkFailsafe()
{
  for (int i=0; i<NUM_MODULES; i++) {
    if (isModulePXX(i)) {
      ModuleData & moduleData = g_model.moduleData[i];
      if (HAS_RF_PROTOCOL_FAILSAFE(moduleData.rfProtocol) && moduleData.failsafeMode == FAILSAFE_NOT_SET) {
        ALERT(STR_FAILSAFEWARN, STR_NO_FAILSAFE, AU_ERROR);
        break;
      }
    }
  }
}
#else
#define checkFailsafe()
#endif
void checkRSSIAlarmsDisabled()
{
  if (g_model.rssiAlarms.disabled) {
    ALERT(STR_RSSIALARM_WARN, STR_NO_RSSIALARM, AU_ERROR);
  }
}

#if defined(GUI)
void checkAll()
{
#if defined(EEPROM_RLC)
  checkLowEEPROM();
#endif

  // we don't check the throttle stick if the radio is not calibrated
  if (g_eeGeneral.chkSum == evalChkSum())
    checkThrottleStick();

  checkSwitches();
  checkFailsafe();
  checkRSSIAlarmsDisabled();

#if defined(SDCARD)
  checkSDVersion();
#endif

  if (g_model.displayChecklist && modelHasNotes()) {
    readModelNotes();
  }

  if (!waitKeysReleased()) {
    showMessageBox(STR_KEYSTUCK);
    tmr10ms_t tgtime = get_tmr10ms() + 500;
    while (tgtime != get_tmr10ms()) {
      RTOS_WAIT_MS(1);
      wdt_reset();
    }
  }

  START_SILENCE_PERIOD();
}
#endif // GUI


#if defined(EEPROM_RLC)
void checkLowEEPROM()
{
  if (g_eeGeneral.disableMemoryWarning) return;
  if (EeFsGetFree() < 100) {
    ALERT(STR_STORAGE_WARNING, STR_EEPROMLOWMEM, AU_ERROR);
  }
}
#endif

bool isThrottleWarningAlertNeeded()
{
  if (g_model.disableThrottleWarning) {
    return false;
  }

  // throttle channel is either the stick according stick mode (already handled in evalInputs)
  // or P1 to P3;
  // in case an output channel is choosen as throttle source (thrTraceSrc>NUM_POTS+NUM_SLIDERS) we assume the throttle stick is the input
  // no other information available at the moment, and good enough to my option (otherwise too much exceptions...)
  uint8_t thrchn = ((g_model.thrTraceSrc==0) || (g_model.thrTraceSrc>NUM_POTS+NUM_SLIDERS)) ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1;

  GET_ADC_IF_MIXER_NOT_RUNNING();
  evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

  int16_t v = calibratedAnalogs[thrchn];
  if (g_model.thrTraceSrc && g_model.throttleReversed) { // TODO : proper review of THR source definition and handling
    v = -v;
  }
  return v > THRCHK_DEADBAND - 1024;
}

void checkThrottleStick()
{
  if (!isThrottleWarningAlertNeeded()) {
    return;
  }

  // first - display warning; also deletes inputs if any have been before
  LED_ERROR_BEGIN();
  RAISE_ALERT(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);

#if defined(PWR_BUTTON_PRESS)
  bool refresh = false;
#endif

  while (!getEvent()) {
    if (!isThrottleWarningAlertNeeded()) {
      return;
    }

#if defined(PWR_BUTTON_PRESS)
    uint32_t power = pwrCheck();
    if (power == e_power_off) {
      break;
    }
    else if (power == e_power_press) {
      refresh = true;
    }
    else if (power == e_power_on && refresh) {
      RAISE_ALERT(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_NONE);
      refresh = false;
    }
#else
    if (pwrCheck() == e_power_off) {
      break;
    }
#endif

    doLoopCommonActions();

    wdt_reset();

    RTOS_WAIT_MS(10);
  }

  LED_ERROR_END();
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) {
    return;
  }

  if (IS_SOUND_OFF()) {
    ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
  }
}

void alert(const char * title, const char * msg , uint8_t sound)
{
  LED_ERROR_BEGIN();

  TRACE("ALERT %s: %s", title, msg);

  RAISE_ALERT(title, msg, STR_PRESSANYKEY, sound);

#if defined(PWR_BUTTON_PRESS)
  bool refresh = false;
#endif

  while (1) {
    RTOS_WAIT_MS(10);

    if (keyDown())  // wait for key release
      break;

    doLoopCommonActions();

    wdt_reset();

    const uint32_t pwr_check = pwrCheck();
    if (pwr_check == e_power_off) {
      drawSleepBitmap();
      boardOff();
      return;   // only happens in SIMU, required for proper shutdown
    }
#if defined(PWR_BUTTON_PRESS)
    else if (pwr_check == e_power_press) {
      refresh = true;
    }
    else if (pwr_check == e_power_on && refresh) {
      RAISE_ALERT(title, msg, STR_PRESSANYKEY, AU_NONE);
      refresh = false;
    }
#endif
  }

  LED_ERROR_END();
}

#if defined(GVARS)
#if NUM_TRIMS == 6
  int8_t trimGvar[NUM_TRIMS] = { -1, -1, -1, -1, -1, -1 };
#elif NUM_TRIMS == 4
  int8_t trimGvar[NUM_TRIMS] = { -1, -1, -1, -1 };
#elif NUM_TRIMS == 2
  int8_t trimGvar[NUM_TRIMS] = { -1, -1 };
#endif
#endif

void checkTrims()
{
  event_t event = getEvent(true);
  if (event && !IS_KEY_BREAK(event)) {
    int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE_TRIMS((uint8_t)k/2);
    uint8_t phase;
    int before;
    bool thro;

    trimsDisplayTimer = 200; // 2 seconds
    trimsDisplayMask |= (1<<idx);

#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      phase = getGVarFlightMode(mixerCurrentFlightMode, trimGvar[idx]);
      before = GVAR_VALUE(trimGvar[idx], phase);
      thro = false;
    }
    else {
      phase = getTrimFlightMode(mixerCurrentFlightMode, idx);
      before = getTrimValue(phase, idx);
      thro = (idx==THR_STICK && g_model.thrTrim);
    }
#else
    phase = getTrimFlightMode(mixerCurrentFlightMode, idx);
    before = getTrimValue(phase, idx);
    thro = (idx==THR_STICK && g_model.thrTrim);
#endif
    int8_t trimInc = g_model.trimInc + 1;
    int8_t v = (trimInc==-1) ? min(32, abs(before)/4+1) : (1 << trimInc); // TODO flash saving if (trimInc < 0)
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
#if defined(GVARS)
    if (TRIM_REUSED(idx)) v = 1;
#endif
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
    bool beepTrim = false;

    if (!thro && before!=0 && ((!(after < 0) == (before < 0)) || after==0)) { //forcing a stop at centerered trim when changing sides
      after = 0;
      beepTrim = true;
      AUDIO_TRIM_MIDDLE();
      pauseEvents(event);
    }

#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      int8_t gvar = trimGvar[idx];
      int16_t vmin = GVAR_MIN + g_model.gvars[gvar].min;
      int16_t vmax = GVAR_MAX - g_model.gvars[gvar].max;
      if (after < vmin) {
        after = vmin;
        beepTrim = true;
        AUDIO_TRIM_MIN();
        killEvents(event);
      }
      else if (after > vmax) {
        after = vmax;
        beepTrim = true;
        AUDIO_TRIM_MAX();
        killEvents(event);
      }

      SET_GVAR_VALUE(gvar, phase, after);
    }
    else
#endif
    {
      if (before>TRIM_MIN && after<=TRIM_MIN) {
        beepTrim = true;
        AUDIO_TRIM_MIN();
        killEvents(event);
      }
      else if (before<TRIM_MAX && after>=TRIM_MAX) {
        beepTrim = true;
        AUDIO_TRIM_MAX();
        killEvents(event);
      }

      if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) {
        if (!g_model.extendedTrims) after = before;
      }

      if (after < TRIM_EXTENDED_MIN) {
        after = TRIM_EXTENDED_MIN;
      }
      else if (after > TRIM_EXTENDED_MAX) {
        after = TRIM_EXTENDED_MAX;
      }

      if (!setTrimValue(phase, idx, after)) {
        // we don't play a beep, so we exit now the function
        return;
      }
    }

    if (!beepTrim) {
      AUDIO_TRIM_PRESS(after);
    }

  }
}

#if !defined(SIMU)
uint16_t s_anaFilt[NUM_ANALOGS];
#endif

#if defined(JITTER_MEASURE)
JitterMeter<uint16_t> rawJitter[NUM_ANALOGS];
JitterMeter<uint16_t> avgJitter[NUM_ANALOGS];
tmr10ms_t jitterResetTime = 0;
#endif

#define JITTER_FILTER_STRENGTH  4         // tune this value, bigger value - more filtering (range: 1-5) (see explanation below)
#define ANALOG_SCALE            1         // tune this value, bigger value - more filtering (range: 0-1) (see explanation below)

#define JITTER_ALPHA            (1<<JITTER_FILTER_STRENGTH)
#define ANALOG_MULTIPLIER       (1<<ANALOG_SCALE)
#define ANA_FILT(chan)          (s_anaFilt[chan] / (JITTER_ALPHA * ANALOG_MULTIPLIER))
#if (JITTER_ALPHA * ANALOG_MULTIPLIER > 32)
  #error "JITTER_FILTER_STRENGTH and ANALOG_SCALE are too big, their summ should be <= 5 !!!"
#endif

#if !defined(SIMU)
uint16_t anaIn(uint8_t chan)
{
  return ANA_FILT(chan);
}

void getADC()
{
#if defined(JITTER_MEASURE)
  if (JITTER_MEASURE_ACTIVE() && jitterResetTime < get_tmr10ms()) {
    // reset jitter measurement every second
    for (uint32_t x=0; x<NUM_ANALOGS; x++) {
      rawJitter[x].reset();
      avgJitter[x].reset();
    }
    jitterResetTime = get_tmr10ms() + 100;  //every second
  }
#endif

  DEBUG_TIMER_START(debugTimerAdcRead);
  adcRead();
  DEBUG_TIMER_STOP(debugTimerAdcRead);

  for (uint8_t x=0; x<NUM_ANALOGS; x++) {
    uint16_t v = getAnalogValue(x) >> (1 - ANALOG_SCALE);

    // Jitter filter:
    //    * pass trough any big change directly
    //    * for small change use Modified moving average (MMA) filter
    //
    // Explanation:
    //
    // Normal MMA filter has this formula:
    //            <out> = ((ALPHA-1)*<out> + <in>)/ALPHA
    //
    // If calculation is done this way with integer arithmetics, then any small change in
    // input signal is lost. One way to combat that, is to rearrange the formula somewhat,
    // to store a more precise (larger) number between iterations. The basic idea is to
    // store undivided value between iterations. Therefore an new variable <filtered> is
    // used. The new formula becomes:
    //           <filtered> = <filtered> - <filtered>/ALPHA + <in>
    //           <out> = <filtered>/ALPHA  (use only when out is needed)
    //
    // The above formula with a maximum allowed ALPHA value (we are limited by
    // the 16 bit s_anaFilt[]) was tested on the radio. The resulting signal still had
    // some jitter (a value of 1 was observed). The jitter might be bigger on other
    // radios.
    //
    // So another idea is to use larger input values for filtering. So instead of using
    // input in a range from 0 to 2047, we use twice larger number (temp[x] is divided less)
    //
    // This also means that ALPHA must be lowered (remember 16 bit limit), but test results
    // have proved that this kind of filtering gives better results. So the recommended values
    // for filter are:
    //     JITTER_FILTER_STRENGTH  4
    //     ANALOG_SCALE            1
    //
    // Variables mapping:
    //   * <in> = v
    //   * <out> = s_anaFilt[x]
    uint16_t previous = s_anaFilt[x] / JITTER_ALPHA;
    uint16_t diff = (v > previous) ? (v - previous) : (previous - v);
    if (!g_eeGeneral.jitterFilter && diff < (10*ANALOG_MULTIPLIER)) { // g_eeGeneral.jitterFilter is inverted, 0 - active
      // apply jitter filter
      s_anaFilt[x] = (s_anaFilt[x] - previous) + v;
    }
    else {
      // use unfiltered value
      s_anaFilt[x] = v * JITTER_ALPHA;
    }

#if defined(JITTER_MEASURE)
    if (JITTER_MEASURE_ACTIVE()) {
      avgJitter[x].measure(ANA_FILT(x));
    }
#endif

    #define ANAFILT_MAX    (2 * RESX * JITTER_ALPHA * ANALOG_MULTIPLIER - 1)
    StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[x];
    if (IS_POT_MULTIPOS(x) && IS_MULTIPOS_CALIBRATED(calib)) {
      // TODO: consider adding another low pass filter to eliminate multipos switching glitches
      uint8_t vShifted = ANA_FILT(x) >> 4;
      s_anaFilt[x] = ANAFILT_MAX;
      for (uint32_t i=0; i<calib->count; i++) {
        if (vShifted < calib->steps[i]) {
          s_anaFilt[x] = (i * ANAFILT_MAX) / calib->count;
          break;
        }
      }
    }
  }
}

#endif // SIMU

uint8_t g_vbat100mV = 0;
uint16_t lightOffCounter;
uint8_t flashCounter = 0;

uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;

uint8_t trimsDisplayTimer = 0;
uint8_t trimsDisplayMask = 0;

void flightReset(uint8_t check)
{
  // we don't reset the whole audio here (the hello.wav would be cut, if a prompt is queued before FlightReset, it should be played)
  // TODO check if the vario / background music are stopped correctly if switching to a model which doesn't have these functions enabled

  if (!IS_MANUAL_RESET_TIMER(0)) {
    timerReset(0);
  }

#if TIMERS > 1
  if (!IS_MANUAL_RESET_TIMER(1)) {
    timerReset(1);
  }
#endif

#if TIMERS > 2
  if (!IS_MANUAL_RESET_TIMER(2)) {
    timerReset(2);
  }
#endif

#if defined(TELEMETRY_FRSKY)
  telemetryReset();
#endif

  s_mixer_first_run_done = false;

  START_SILENCE_PERIOD();

  RESET_THR_TRACE();

  logicalSwitchesReset();

  if (check) {
    checkAll();
  }
}

#if defined(THRTRACE)
uint8_t  s_traceBuf[MAXTRACE];
uint16_t s_traceWr;
uint8_t  s_cnt_10s;
uint16_t s_cnt_samples_thr_10s;
uint16_t s_sum_samples_thr_10s;
#endif

void evalTrims()
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<NUM_TRIMS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
    if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = trim*2;
  }
}

uint8_t s_mixer_first_run_done = false;

void doMixerCalculations()
{
  static tmr10ms_t lastTMR = 0;

  tmr10ms_t tmr10ms = get_tmr10ms();

#if defined(DEBUG_LATENCY)
  static tmr10ms_t lastLatencyToggle = 0;
  if (tmr10ms - lastLatencyToggle >= 10) {
    lastLatencyToggle = tmr10ms;
    latencyToggleSwitch ^= 1;
    if (latencyToggleSwitch)
      sportUpdatePowerOn();
    else
      sportUpdatePowerOff();
  }
#endif

  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  // handle tick10ms overrun
  // correct overflow handling costs a lot of code; happens only each 11 min;
  // therefore forget the exact calculation and use only 1 instead; good compromise
  lastTMR = tmr10ms;

  DEBUG_TIMER_START(debugTimerGetAdc);
  getADC();
  DEBUG_TIMER_STOP(debugTimerGetAdc);

  DEBUG_TIMER_START(debugTimerGetSwitches);
  getSwitchesPosition(!s_mixer_first_run_done);
  DEBUG_TIMER_STOP(debugTimerGetSwitches);

#if defined(PCBSKY9X) && !defined(SIMU)
  Current_analogue = (Current_analogue*31 + s_anaFilt[8] ) >> 5 ;
  if (Current_analogue > Current_max)
    Current_max = Current_analogue ;
#endif


  DEBUG_TIMER_START(debugTimerEvalMixes);
  evalMixes(tick10ms);
  DEBUG_TIMER_STOP(debugTimerEvalMixes);

  DEBUG_TIMER_START(debugTimerMixes10ms);
  if (tick10ms) {
    /* Throttle trace */
    int16_t val;

    if (g_model.thrTraceSrc > NUM_POTS+NUM_SLIDERS) {
      uint8_t ch = g_model.thrTraceSrc-NUM_POTS-NUM_SLIDERS-1;
      val = channelOutputs[ch];

      LimitData * lim = limitAddress(ch);
      int16_t gModelMax = LIMIT_MAX_RESX(lim);
      int16_t gModelMin = LIMIT_MIN_RESX(lim);

      if (lim->revert)
        val = -val + gModelMax;
      else
        val = val - gModelMin;

#if defined(PPM_LIMITS_SYMETRICAL)
      if (lim->symetrical) {
        val -= calc1000toRESX(lim->offset);
      }
#endif

      gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
      // usually max is 1024 min is -1024 --> max-min = 2048 full range

      if (gModelMax != 0 && gModelMax != 2048)
        val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs

      if (val < 0)
        val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
    }
    else {
      val = RESX + calibratedAnalogs[g_model.thrTraceSrc == 0 ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1];
    }

    val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)

    evalTimers(val, tick10ms);

    static uint8_t  s_cnt_100ms;
    static uint8_t  s_cnt_1s;
    static uint8_t  s_cnt_samples_thr_1s;
    static uint16_t s_sum_samples_thr_1s;

    s_cnt_samples_thr_1s++;
    s_sum_samples_thr_1s+=val;

    if ((s_cnt_100ms += tick10ms) >= 10) { // 0.1sec
      s_cnt_100ms -= 10;
      s_cnt_1s += 1;

      logicalSwitchesTimerTick();
      checkTrainerSignalWarning();

      if (s_cnt_1s >= 10) { // 1sec
        s_cnt_1s -= 10;
        sessionTimer += 1;
        inactivity.counter++;
        if ((((uint8_t)inactivity.counter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && g_vbat100mV>50 && inactivity.counter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
          AUDIO_INACTIVITY();

#if defined(AUDIO)
        if (mixWarning & 1) if ((sessionTimer&0x03)==0) AUDIO_MIX_WARNING(1);
        if (mixWarning & 2) if ((sessionTimer&0x03)==1) AUDIO_MIX_WARNING(2);
        if (mixWarning & 4) if ((sessionTimer&0x03)==2) AUDIO_MIX_WARNING(3);
#endif

        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
        if (val) s_timeCumThr += 1;
        s_sum_samples_thr_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this

#if defined(THRTRACE)
        // throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
        // in case buffer runs out, it wraps around
        // resolution for y axis is only 32, therefore no higher value makes sense
        s_cnt_samples_thr_10s += s_cnt_samples_thr_1s;
        s_sum_samples_thr_10s += s_sum_samples_thr_1s;

        if (++s_cnt_10s >= 10) { // 10s
          s_cnt_10s -= 10;
          val = s_sum_samples_thr_10s / s_cnt_samples_thr_10s;
          s_sum_samples_thr_10s = 0;
          s_cnt_samples_thr_10s = 0;
          s_traceBuf[s_traceWr % MAXTRACE] = val;
          s_traceWr++;
        }
#endif

        s_cnt_samples_thr_1s = 0;
        s_sum_samples_thr_1s = 0;
      }
    }

#if defined(PXX) || defined(DSM2)
    static uint8_t countRangecheck = 0;
    for (uint8_t i=0; i<NUM_MODULES; ++i) {
#if defined(MULTIMODULE)
      if (moduleState[i].mode >= MODULE_MODE_BEEP_FIRST || multiModuleStatus.isBinding()) {
#else
      if (moduleState[i].mode >= MODULE_MODE_BEEP_FIRST) {
#endif
        if (++countRangecheck >= 250) {
          countRangecheck = 0;
          AUDIO_PLAY(AU_SPECIAL_SOUND_CHEEP);
        }
      }
    }
#endif

    checkTrims();
  }
  DEBUG_TIMER_STOP(debugTimerMixes10ms);

  s_mixer_first_run_done = true;
}

#if !defined(OPENTX_START_DEFAULT_ARGS)
  #define OPENTX_START_DEFAULT_ARGS  0
#endif

void opentxStart(const uint8_t startOptions = OPENTX_START_DEFAULT_ARGS)
{
  TRACE("opentxStart(%u)", startOptions);

  uint8_t calibration_needed = !(startOptions & OPENTX_START_NO_CALIBRATION) && (g_eeGeneral.chkSum != evalChkSum());

#if defined(GUI)
  if (!calibration_needed && !(startOptions & OPENTX_START_NO_SPLASH)) {
    doSplash();
  }
#endif

#if defined(DEBUG_TRACE_BUFFER)
  trace_event(trace_start, 0x12345678);
#endif

#if defined(PCBSKY9X) && defined(SDCARD) && !defined(SIMU)
  for (int i=0; i<500 && !Card_initialized; i++) {
    RTOS_WAIT_MS(2); // 2ms
  }
#endif

#if defined(NIGHTLY_BUILD_WARNING)
  ALERT(STR_NIGHTLY_WARNING, TR_NIGHTLY_NOTSAFE, AU_ERROR);
#endif

#if defined(GUI)
  if (calibration_needed) {
    chainMenu(menuFirstCalib);
  }
  else if (!(startOptions & OPENTX_START_NO_CHECKS)) {
    checkAlarm();
    checkAll();
    PLAY_MODEL_NAME();
  }
#endif
}

void opentxClose(uint8_t shutdown)
{
  TRACE("opentxClose");

  if (shutdown) {
    watchdogSuspend(2000/*20s*/);
    pausePulses();   // stop mixer task to disable trims processing while in shutdown
    AUDIO_BYE();
#if defined(TELEMETRY_FRSKY)
    // TODO needed? telemetryEnd();
#endif
#if defined(LUA)
    luaClose(&lsScripts);
#if defined(COLORLCD)
    luaClose(&lsWidgets);
#endif
#endif
#if defined(HAPTIC)
    hapticOff();
#endif
  }

#if defined(SDCARD)
  logsClose();
#endif

  storageFlushCurrentModel();

  if (sessionTimer > 0) {
    g_eeGeneral.globalTimer += sessionTimer;
    sessionTimer = 0;
  }

#if defined(PCBSKY9X)
  uint32_t mAhUsed = g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.txCurrentCalibration) / 8192 / 36;
  if (g_eeGeneral.mAhUsed != mAhUsed) {
    g_eeGeneral.mAhUsed = mAhUsed;
  }
#endif

  g_eeGeneral.unexpectedShutdown = 0;
  storageDirty(EE_GENERAL);
  storageCheck(true);

  while (IS_PLAYING(ID_PLAY_PROMPT_BASE + AU_BYE)) {
    RTOS_WAIT_MS(10);
  }

  RTOS_WAIT_MS(100);

#if defined(SDCARD)
  sdDone();
#endif
}

#if defined(STM32)
void opentxResume()
{
  TRACE("opentxResume");

  menuHandlers[0] = menuMainView;

  sdMount();
  storageReadAll();

#if defined(COLORLCD)
  loadTheme();
  loadFontCache();
#endif

  // removed to avoid the double warnings (throttle, switch, etc.)
  // opentxStart(OPENTX_START_NO_SPLASH | OPENTX_START_NO_CALIBRATION | OPENTX_START_NO_CHECKS);

  referenceSystemAudioFiles();

  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    storageDirty(EE_GENERAL);
  }
}
#endif

#define INSTANT_TRIM_MARGIN 10 /* around 1% */

void instantTrim()
{
  int16_t  anas_0[MAX_INPUTS];
  evalInputs(e_perout_mode_notrainer | e_perout_mode_nosticks);
  memcpy(anas_0, anas, sizeof(anas_0));

  evalInputs(e_perout_mode_notrainer);

  for (uint8_t stick=0; stick<NUM_STICKS; stick++) {
    if (stick!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightMode(mixerCurrentFlightMode, stick);
      int16_t delta = 0;
      for (int e=0; e<MAX_EXPOS; e++) {
        ExpoData * ed = expoAddress(e);
        if (!EXPO_VALID(ed)) break; // end of list
        if (ed->srcRaw-MIXSRC_Rud == stick) {
          delta = anas[ed->chn] - anas_0[ed->chn];
          break;
        }
      }
      if (abs(delta) >= INSTANT_TRIM_MARGIN) {
        int16_t trim = limit<int16_t>(TRIM_EXTENDED_MIN, (delta + trims[stick]) / 2, TRIM_EXTENDED_MAX);
        setTrimValue(trim_phase, stick, trim);
      }
    }
  }

  storageDirty(EE_MODEL);
  AUDIO_WARNING2();
}

void copySticksToOffset(uint8_t ch)
{
  pauseMixerCalculations();
  int32_t zero = (int32_t)channelOutputs[ch];

  evalFlightModeMixes(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
  int32_t val = chans[ch];
  LimitData *ld = limitAddress(ch);
  limit_min_max_t lim = LIMIT_MIN(ld);
  if (val < 0) {
    val = -val;
    lim = LIMIT_MIN(ld);
  }
  zero = (zero*256000 - val*lim) / (1024*256-val);
  ld->offset = (ld->revert ? -zero : zero);
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void copyTrimsToOffset(uint8_t ch)
{
  int16_t zero;

  pauseMixerCalculations();

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  zero = applyLimits(ch, chans[ch]);

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  int16_t output = applyLimits(ch, chans[ch]) - zero;
  int16_t v = g_model.limitData[ch].offset;
  if (g_model.limitData[ch].revert) output = -output;
  v += (output * 125) / 128;
  g_model.limitData[ch].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire

  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[MAX_OUTPUT_CHANNELS];

  pauseMixerCalculations();

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<MAX_OUTPUT_CHANNELS; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  for (uint8_t i=0; i<MAX_OUTPUT_CHANNELS; i++) {
    int16_t output = applyLimits(i, chans[i]) - zeros[i];
    int16_t v = g_model.limitData[i].offset;
    if (g_model.limitData[i].revert) output = -output;
    v += (output * 125) / 128;
    g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
  }

  // reset all trims, except throttle (if throttle trim)
  for (uint8_t i=0; i<NUM_TRIMS; i++) {
    if (i != THR_STICK || !g_model.thrTrim) {
      int16_t original_trim = getTrimValue(mixerCurrentFlightMode, i);
      for (uint8_t fm=0; fm<MAX_FLIGHT_MODES; fm++) {
        trim_t trim = getRawTrimValue(fm, i);
        if (trim.mode / 2 == fm)
          setTrimValue(fm, i, trim.value - original_trim);
      }
    }
  }

  resumeMixerCalculations();

  storageDirty(EE_MODEL);
  AUDIO_WARNING2();
}

#if defined(ROTARY_ENCODER_NAVIGATION)
  volatile rotenc_t rotencValue = 0;
  uint8_t rotencSpeed;
#endif

void opentxInit()
{
  TRACE("opentxInit");

#if defined(GUI)
  menuHandlers[0] = menuMainView;
  #if MENUS_LOCK != 2/*no menus*/
    menuHandlers[1] = menuModelSelect;
  #endif
#endif

#if defined(RTCLOCK) && !defined(COPROCESSOR)
  rtcInit(); // RTC must be initialized before rambackupRestore() is called
#endif

#if defined(EEPROM)
  storageReadRadioSettings();
#endif

  // Radios handle UNEXPECTED_SHUTDOWN() differently:
  //  * radios with WDT and EEPROM and CPU controlled power use Reset status register
  //    and eeGeneral.unexpectedShutdown
  //  * radios with SDCARD model storage use Reset status register and special
  //    variables in RAM. They can not use eeGeneral.unexpectedShutdown
  //  * radios without CPU controlled power can only use Reset status register (if available)
  if (UNEXPECTED_SHUTDOWN()) {
    TRACE("Unexpected Shutdown detected");
    unexpectedShutdown = 1;
  }

#if defined(SDCARD) && !defined(PCBMEGA2560)
  // SDCARD related stuff, only done if not unexpectedShutdown
  if (!unexpectedShutdown) {
    sdInit();
    logsInit();
  }
#endif

#if defined(EEPROM)
  storageReadCurrentModel();
#endif

#if defined(COLORLCD)
  if (!unexpectedShutdown) {
    // g_model.topbarData is still zero here (because it was not yet read from SDCARD),
    // but we only remember the pointer to in in constructor.
    // The storageReadAll() needs topbar object, so it must be created here
#if __clang__
// clang does not like this at all, turn into a warning so that -Werror does not stop here
// taking address of packed member 'topbarData' of class or structure 'ModelData' may result in an unaligned pointer value [-Werror,-Waddress-of-packed-member]
#pragma clang diagnostic push
#pragma clang diagnostic warning "-Waddress-of-packed-member"
#endif
    topbar = new Topbar(&g_model.topbarData);
#if __clang__
// Restore warnings
#pragma clang diagnostic pop
#endif

    // lua widget state must also be prepared before the call to storageReadAll()
    LUA_INIT_THEMES_AND_WIDGETS();
  }
#endif

  // handling of storage for radios that have no EEPROM
#if !defined(EEPROM)
#if defined(RAMBACKUP)
  if (unexpectedShutdown) {
    // SDCARD not available, try to restore last model from RAM
    TRACE("rambackupRestore");
    rambackupRestore();
  }
  else {
    storageReadAll();
  }
#else
  storageReadAll();
#endif
#endif  // #if !defined(EEPROM)

#if defined(AUX_SERIAL)
  auxSerialInit(g_eeGeneral.auxSerialMode, modelTelemetryProtocol());
#endif

#if defined(PCBTARANIS)
  BACKLIGHT_ENABLE();
#endif

#if MENUS_LOCK == 1
  getMovedSwitch();
  if (TRIMS_PRESSED() && g_eeGeneral.switchUnlockStates==switches_states) {
    readonly = false;
  }
#endif

  currentSpeakerVolume = requiredSpeakerVolume = g_eeGeneral.speakerVolume + VOLUME_LEVEL_DEF;
#if !defined(SOFTWARE_VOLUME)
  setScaledVolume(currentSpeakerVolume);
#endif

  referenceSystemAudioFiles();
  audioQueue.start();
  BACKLIGHT_ENABLE();

#if defined(PCBSKY9X)
  // Set ADC gains here
  setSticksGain(g_eeGeneral.sticksGain);
#endif

#if defined(PCBSKY9X) && defined(BLUETOOTH)
  btInit();
#endif

#if defined(COLORLCD)
  loadTheme();
  loadFontCache();
#endif

  if (g_eeGeneral.backlightMode != e_backlight_mode_off) {
    // on Tx start turn the light on
    backlightOn();
  }

  if (!unexpectedShutdown) {
    opentxStart();
  }

  // TODO Horus does not need this
  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    storageDirty(EE_GENERAL);
  }

#if defined(GUI)
  lcdSetContrast();
#endif
  backlightOn();

  startPulses();

  wdt_enable(WDTO_500MS);
}

#if defined(SIMU)
void simuMain()
#else
int main()
#endif
{
#if defined(STM32)
  TRACE("reusableBuffer: modelSel=%d, moduleSetup=%d, calib=%d, sdManager=%d, hardwareAndSettings=%d, spectrumAnalyser=%d, usb=%d",
        sizeof(reusableBuffer.modelsel),
        sizeof(reusableBuffer.moduleSetup),
        sizeof(reusableBuffer.calib),
        sizeof(reusableBuffer.sdManager),
        sizeof(reusableBuffer.hardwareAndSettings),
        sizeof(reusableBuffer.spectrumAnalyser),
        sizeof(reusableBuffer.MSC_BOT_Data));
#endif

  // G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
  // important to disable it before commencing with system initialisation (or
  // we could put a bunch more wdt_reset()s in. But I don't like that approach
  // during boot up.)
#if defined(PCBTARANIS)
  g_eeGeneral.contrast = LCD_CONTRAST_DEFAULT;
#endif

  boardInit();

#if defined(COLORLCD)
  loadFonts();
#endif

#if !defined(SIMU)
  stackPaint();
#endif

#if defined(SPLASH) && (defined(PCBTARANIS) || defined(PCBHORUS))
  drawSplash();
#endif

#if defined(PCBHORUS)
  if (!IS_FIRMWARE_COMPATIBLE_WITH_BOARD()) {
    runFatalErrorScreen(STR_WRONG_PCBREV);
  }
#endif

#if !defined(EEPROM)
  if (!SD_CARD_PRESENT() && !UNEXPECTED_SHUTDOWN()) {
    runFatalErrorScreen(STR_NO_SDCARD);
  }
#endif

  tasksStart();
}

#if !defined(SIMU)
#if defined(PWR_BUTTON_PRESS)
uint32_t pwr_press_time = 0;

uint32_t pwrPressedDuration()
{
  if (pwr_press_time == 0) {
    return 0;
  }
  else {
    return get_tmr10ms() - pwr_press_time;
  }
}

uint32_t pwrCheck()
{
  const char * message = NULL;

  enum PwrCheckState {
    PWR_CHECK_ON,
    PWR_CHECK_OFF,
    PWR_CHECK_PAUSED,
  };

  static uint8_t pwr_check_state = PWR_CHECK_ON;

  if (pwr_check_state == PWR_CHECK_OFF) {
    return e_power_off;
  }
  else if (pwrPressed()) {
    if (TELEMETRY_STREAMING()) {
      message = STR_MODEL_STILL_POWERED;
    }
    if (pwr_check_state == PWR_CHECK_PAUSED) {
      // nothing
    }
    else if (pwr_press_time == 0) {
      pwr_press_time = get_tmr10ms();
      if (message && !g_eeGeneral.disableRssiPoweroffAlarm) {
        audioEvent(AU_MODEL_STILL_POWERED);
      }
    }
    else {
      inactivity.counter = 0;
      if (g_eeGeneral.backlightMode != e_backlight_mode_off) {
        BACKLIGHT_ENABLE();
      }
      if (get_tmr10ms() - pwr_press_time > PWR_PRESS_SHUTDOWN_DELAY) {
#if defined(SHUTDOWN_CONFIRMATION)
        while (1) {
#else
        while ((TELEMETRY_STREAMING() && !g_eeGeneral.disableRssiPoweroffAlarm)) {
#endif
          lcdRefreshWait();
          lcdClear();

          POPUP_CONFIRMATION(STR_MODEL_SHUTDOWN, nullptr);
          SET_WARNING_INFO(STR_MODEL_STILL_POWERED, sizeof(TR_MODEL_STILL_POWERED), 0);
          event_t evt = getEvent(false);
          DISPLAY_WARNING(evt);
          lcdRefresh();

          if (warningResult) {
            pwr_check_state = PWR_CHECK_OFF;
            return e_power_off;
          }
          else if (!warningText) {
            // shutdown has been cancelled
            pwr_check_state = PWR_CHECK_PAUSED;
            return e_power_on;
          }
        }
        haptic.play(15, 3, PLAY_NOW);
        pwr_check_state = PWR_CHECK_OFF;
        return e_power_off;
      }
      else {
        drawShutdownAnimation(pwrPressedDuration(), message);
        return e_power_press;
      }
    }
  }
  else {
    pwr_check_state = PWR_CHECK_ON;
    pwr_press_time = 0;
  }

  return e_power_on;
}
#else
uint32_t pwrCheck()
{
#if defined(SOFT_PWR_CTRL)
  if (pwrPressed()) {
    return e_power_on;
  }
#endif

  if (usbPlugged()) {
    return e_power_usb;
  }

#if defined(TRAINER_PWR)
  if (TRAINER_CONNECTED()) {
    return e_power_trainer;
  }
#endif

  if (!g_eeGeneral.disableRssiPoweroffAlarm) {
    if (TELEMETRY_STREAMING()) {
      RAISE_ALERT(STR_MODEL, STR_MODEL_STILL_POWERED, STR_PRESS_ENTER_TO_CONFIRM, AU_MODEL_STILL_POWERED);
      while (TELEMETRY_STREAMING()) {
        resetForcePowerOffRequest();
        RTOS_WAIT_MS(20);
        if (pwrPressed()) {
          return e_power_on;
        }
        else if (readKeys() == (1 << KEY_ENTER)) {
          return e_power_off;
        }
      }
    }
  }

  return e_power_off;
}
#endif  // defined(PWR_BUTTON_PRESS)
#endif  // !defined(SIMU)
