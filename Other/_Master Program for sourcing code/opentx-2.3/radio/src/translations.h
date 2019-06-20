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

#ifndef _TRANSLATIONS_H_
#define _TRANSLATIONS_H_

#include "translations/untranslated.h"

#if defined(TRANSLATIONS_FR)
#include "translations/fr.h"
#define LEN_SPECIAL_CHARS 3
#elif defined(TRANSLATIONS_IT)
#include "translations/it.h"
#define LEN_SPECIAL_CHARS 1
#elif defined(TRANSLATIONS_SE)
#include "translations/se.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_FI)
#include "translations/fi.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_DE)
#include "translations/de.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_CZ)
#include "translations/cz.h"
#define LEN_SPECIAL_CHARS 17
#elif defined(TRANSLATIONS_ES)
#include "translations/es.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_PL)
#include "translations/pl.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_PT)
#include "translations/pt.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_NL)
#include "translations/nl.h"
#define LEN_SPECIAL_CHARS 0
#else
#include "translations/en.h"
#define LEN_SPECIAL_CHARS 0
#endif

#define PSIZE(x) ( sizeof(x) - 1 )

#if defined(PCBTARANIS)
#define HINT(s) " " s " "
#else
#define HINT(s) s
#endif

#if LCD_W >= 480
  #define TR3(x, y, z) z
  #define TR2(x, y) y
#elif LCD_W >= 212
  #define TR3(x, y, z) y
  #define TR2(x, y) y
#else
  #define TR3(x, y, z) x
  #define TR2(x, y) x
#endif

#define TR(x, y) TR2(x, y) // for compatibility

#if defined(COLORLCD)
  #define BUTTON(x) x
#else
  #define BUTTON(x) "[" x "]"
#endif

#if (LCD_W == 212)
 #define LCDW_128_480_LINEBREAK        ""
#else
 #define LCDW_128_480_LINEBREAK        "\036"
#endif

extern const char STR_OFFON[];
extern const char STR_MMMINV[];
extern const char STR_VBEEPMODE[];
extern const char STR_TRNMODE[];
extern const char STR_TRNCHN[];
extern const char STR_AUX_SERIAL_MODES[];
extern const char STR_SWTYPES[];
extern const char STR_POTTYPES[];
extern const char STR_SLIDERTYPES[];
extern const char STR_VTRIMINC[];
extern const char STR_VDISPLAYTRIMS[];
extern const char STR_RETA123[];
extern const char STR_VPROTOS[];

#if defined(PCBSKY9X) && defined(REVX)
extern const char STR_VOUTPUT_TYPE[];
#endif

extern const char STR_VBLMODE[];
extern const char STR_VCURVEFUNC[];
extern const char STR_VMLTPX[];
extern const char STR_VMLTPX2[];
extern const char STR_VMIXTRIMS[];
extern const char STR_VCSWFUNC[];
extern const char STR_VFSWFUNC[];
extern const char STR_VFSWRESET[];
extern const char STR_FUNCSOUNDS[];
extern const char STR_VTELEMUNIT[];
extern const char STR_GPSFORMAT[];
extern const char STR_VTELEMSCREENTYPE[];

#if defined(HELI)
extern const char STR_VSWASHTYPE[];
#endif

extern const char STR_VKEYS[];
extern const char STR_VSWITCHES[];
extern const char STR_VSRCRAW[];

#if defined(TRANSLATIONS_CZ)
extern const char STR_INPUTNAMES[];
#endif

extern const char STR_VTMRMODES[];

#if defined(ROTARY_ENCODER_NAVIGATION)
extern const char STR_VRENCODERS[];
#endif

extern const char STR_VPERSISTENT[];

extern const char STR_VLCD[];
extern const char STR_VUNITSSYSTEM[];
extern const char STR_VBEEPCOUNTDOWN[];
extern const char STR_VVARIOCENTER[];

extern const char STR_COUNTRYCODES[];
extern const char STR_USBMODES[];
extern const char STR_JACKMODES[];
extern const char STR_VFAILSAFE[];

extern const char STR_VTRAINERMODES[];
extern const char STR_EXTERNAL_MODULE_PROTOCOLS[];
extern const char STR_INTERNAL_MODULE_PROTOCOLS[];
extern const char STR_R9M_REGION[];
extern const char STR_R9M_FCC_POWER_VALUES[];
extern const char STR_R9M_LBT_POWER_VALUES[];
extern const char STR_R9M_LITE_FCC_POWER_VALUES[];
extern const char STR_R9M_LITE_LBT_POWER_VALUES[];
extern const char STR_TELEMETRY_PROTOCOLS[];
extern const char STR_ACCST_RF_PROTOCOLS[];
extern const char STR_ISRM_PXX2_RF_PROTOCOLS[];
extern const char STR_R9M_PXX2_RF_PROTOCOLS[];
extern const char STR_DSM_PROTOCOLS[];

#if defined(MULTIMODULE)
extern const char STR_MULTI_PROTOCOLS[];
#endif

extern const char STR_CURVE_TYPES[];
extern const char STR_VSENSORTYPES[];
extern const char STR_VFORMULAS[];
extern const char STR_VPREC[];
extern const char STR_VCELLINDEX[];
extern const char STR_VANTENNATYPES[];

#if defined(BLUETOOTH)
extern const char STR_BLUETOOTH[];
extern const char STR_BLUETOOTH_DISC[];
extern const char STR_BLUETOOTH_INIT[];
extern const char STR_BLUETOOTH_DIST_ADDR[];
extern const char STR_BLUETOOTH_LOCAL_ADDR[];
extern const char STR_BLUETOOTH_PIN_CODE[];
extern const char STR_BLUETOOTH_MODES[];
#endif

#define NO_INDENT(x) (x)+LEN_INDENT

extern const char STR_POPUPS_ENTER_EXIT[];
extern const char STR_OK[];
extern const char STR_EXIT[];

#if !defined(PCBHORUS)
extern const char STR_MENUWHENDONE[];
#endif

extern const char STR_FREE[];
#define LEN_FREE PSIZE(TR_FREE)
extern const char STR_DELETEMODEL[];
extern const char STR_COPYINGMODEL[];
extern const char STR_MOVINGMODEL[];
extern const char STR_LOADINGMODEL[];
extern const char STR_NAME[];
extern const char STR_BITMAP[];
extern const char STR_TIMER[];
extern const char STR_ELIMITS[];
extern const char STR_ETRIMS[];
extern const char STR_TRIMINC[];
extern const char STR_DISPLAY_TRIMS[];
extern const char STR_TTRACE[];
extern const char STR_TTRIM[];
extern const char STR_BEEPCTR[];
extern const char STR_USE_GLOBAL_FUNCS[];

#if defined(PCBSKY9X) && defined(REVX)
  extern const char STR_OUTPUT_TYPE[];
#endif

extern const char STR_PPMFRAME[];
extern const char STR_REFRESHRATE[];
extern const char STR_MS[];
extern const char STR_SWITCH[];
extern const char STR_TRIMS[];
extern const char STR_FADEIN[];
extern const char STR_FADEOUT[];
extern const char STR_CHECKTRIMS[];
extern const char STR_SWASHTYPE[];
extern const char STR_COLLECTIVE[];
extern const char STR_AILERON[];
extern const char STR_ELEVATOR[];
extern const char STR_SWASHRING[];
extern const char STR_MODE[];

#if defined(AUDIO) && defined(BUZZER)
extern const char STR_SPEAKER[];
extern const char STR_BUZZER[];
#else
#define STR_SPEAKER STR_MODE
#define STR_BUZZER  STR_MODE
#endif

extern const char STR_NOFREEEXPO[];
extern const char STR_NOFREEMIXER[];
extern const char STR_SOURCE[];
extern const char STR_WEIGHT[];
extern const char STR_SIDE[];
extern const char STR_OFFSET[];
extern const char STR_TRIM[];
extern const char STR_DREX[];
extern const char STR_CURVE[];
extern const char STR_FLMODE[];
extern const char STR_MIXWARNING[];
extern const char STR_OFF[];
extern const char STR_MULTPX[];
extern const char STR_DELAYDOWN[];
extern const char STR_DELAYUP[];
extern const char STR_SLOWDOWN[];
extern const char STR_SLOWUP[];
extern const char STR_MIXER[];
extern const char STR_CV[];
extern const char STR_GV[];
extern const char STR_RANGE[];
extern const char STR_CENTER[];
extern const char STR_ALARM[];
extern const char STR_BLADES[];
extern const char STR_SCREEN[];
extern const char STR_SOUND_LABEL[];
extern const char STR_LENGTH[];
extern const char STR_BEEP_LENGTH[];
extern const char STR_SPKRPITCH[];
extern const char STR_HAPTIC_LABEL[];
extern const char STR_HAPTICSTRENGTH[];
extern const char STR_GYRO_LABEL[];
extern const char STR_GYRO_OFFSET[];
extern const char STR_GYRO_MAX[];
extern const char STR_CONTRAST[];
extern const char STR_ALARMS_LABEL[];
extern const char STR_BATTERY_RANGE[];
extern const char STR_BATTERYWARNING[];
extern const char STR_INACTIVITYALARM[];
extern const char STR_MEMORYWARNING[];
extern const char STR_ALARMWARNING[];
extern const char STR_RSSISHUTDOWNALARM[];
extern const char STR_MODEL_STILL_POWERED[];
extern const char STR_MODEL_SHUTDOWN[];
extern const char STR_PRESS_ENTER_TO_CONFIRM[];
extern const char STR_THROTTLEREVERSE[];
extern const char STR_MINUTEBEEP[];
extern const char STR_BEEPCOUNTDOWN[];
extern const char STR_PERSISTENT[];
extern const char STR_BACKLIGHT_LABEL[];
extern const char STR_BLDELAY[];

#if defined(PWM_BACKLIGHT) || defined(PCBHORUS)
extern const char STR_BLONBRIGHTNESS[];
extern const char STR_BLOFFBRIGHTNESS[];
#endif

extern const char STR_SPLASHSCREEN[];
extern const char STR_THROTTLEWARNING[];
extern const char STR_SWITCHWARNING[];
extern const char STR_POTWARNINGSTATE[];
extern const char STR_POTWARNING[];
extern const char STR_SLIDERWARNING[];
extern const char STR_TIMEZONE[];
extern const char STR_ADJUST_RTC[];
extern const char STR_GPS[];
extern const char STR_GPSCOORD[];
extern const char STR_VARIO[];
extern const char STR_PITCH_AT_ZERO[];
extern const char STR_PITCH_AT_MAX[];
extern const char STR_REPEAT_AT_ZERO[];
extern const char STR_RXCHANNELORD[];
extern const char STR_STICKS[];
extern const char STR_POTS[];
extern const char STR_SWITCHES[];
extern const char STR_SWITCHES_DELAY[];
extern const char STR_SLAVE[];
extern const char STR_MODESRC[];
extern const char STR_MULTIPLIER[];
#define LEN_MULTIPLIER PSIZE(TR_MULTIPLIER)
extern const char STR_CAL[];
extern const char STR_VTRIM[];
extern const char STR_MENUTOSTART[];
extern const char STR_SETMIDPOINT[];
extern const char STR_MOVESTICKSPOTS[];
extern const char STR_TX[];
#define STR_RX (STR_TX+OFS_RX)
extern const char STR_NODATA[];
extern const char STR_US[];
extern const char STR_TMIXMAXMS[];
extern const char STR_FREE_STACK[];
extern const char STR_MENUTORESET[];
extern const char STR_PPM_TRAINER[];
extern const char STR_CH[];
extern const char STR_MODEL[];
extern const char STR_FM[];
extern const char STR_EEPROMLOWMEM[];
extern const char STR_PRESSANYKEYTOSKIP[];
extern const char STR_THROTTLENOTIDLE[];
extern const char STR_ALARMSDISABLED[];
extern const char STR_PRESSANYKEY[];
extern const char STR_BAD_RADIO_DATA[];
extern const char STR_STORAGE_FORMAT[];
extern const char STR_EEPROMOVERFLOW[];
extern const char STR_TRIMS2OFFSETS[];
extern const char STR_CHANNELS2FAILSAFE[];
extern const char STR_CHANNEL2FAILSAFE[];
extern const char STR_MENURADIOSETUP[];
extern const char STR_MENUTRAINER[];
extern const char STR_MENUSPECIALFUNCS[];
extern const char STR_MENUVERSION[];
extern const char STR_MENU_RADIO_SWITCHES[];
extern const char STR_MENU_RADIO_ANALOGS[];
extern const char STR_MENUCALIBRATION[];
extern const char STR_MENUMODELSEL[];
extern const char STR_MENUSETUP[];
extern const char STR_MENUFLIGHTMODE[];
extern const char STR_MENUFLIGHTMODES[];
extern const char STR_MENUHELISETUP[];
extern const char STR_MENUINPUTS[];
extern const char STR_MENULIMITS[];
extern const char STR_MENUCURVES[];
extern const char STR_MENUCURVE[];
extern const char STR_MENULOGICALSWITCH[];
extern const char STR_MENULOGICALSWITCHES[];
extern const char STR_MENUCUSTOMFUNC[];
extern const char STR_MENUCUSTOMSCRIPTS[];
extern const char STR_MENUTELEMETRY[];
extern const char STR_MENUTEMPLATES[];
extern const char STR_MENUSTAT[];
extern const char STR_MENUDEBUG[];
extern const char * const STR_MONITOR_CHANNELS[];
extern const char STR_MONITOR_SWITCHES[];
extern const char STR_MONITOR_OUTPUT_DESC[];
extern const char STR_MONITOR_MIXER_DESC[];
extern const char STR_MENUGLOBALVARS[];
extern const char STR_AND_SWITCH[];
extern const char STR_SF[];
extern const char STR_GF[];

#if defined(MULTIMODULE)
extern const char STR_MULTI_CUSTOM[];
extern const char STR_MULTI_OPTION[];
extern const char STR_MULTI_VIDFREQ[];
extern const char STR_MULTI_RFTUNE[];
extern const char STR_MULTI_TELEMETRY[];
extern const char STR_MULTI_AUTOBIND[];
extern const char STR_MULTI_DSM_AUTODTECT[];
extern const char STR_MULTI_LOWPOWER[];
extern const char STR_MODULE_NO_SERIAL_MODE[];
extern const char STR_MODULE_NO_INPUT[];
extern const char STR_MODULE_WAITFORBIND[];
extern const char STR_MODULE_NO_TELEMETRY[];
extern const char STR_MODULE_BINDING[];
extern const char STR_PROTOCOL_INVALID[];
extern const char STR_MODULE_STATUS[];
extern const char STR_MODULE_SYNC[];
extern const char STR_MULTI_SERVOFREQ[];
#if LCD_W < 212
extern const char STR_SUBTYPE[];
#endif
#endif

#if defined(DSM2) || defined(PXX)
extern const char STR_RECEIVER_NUM[];
extern const char STR_RECEIVER[];
extern const char STR_REBIND[];
extern const char STR_REG_OK[];
extern const char STR_BIND_OK[];
#endif

extern const char STR_INTERNALRF[];
extern const char STR_EXTERNALRF[];
extern const char STR_MODULE_TELEMETRY[];
extern const char STR_MODULE_TELEM_ON[];
extern const char STR_FAILSAFE[];
extern const char STR_FAILSAFESET[];
extern const char STR_RECEIVER[];
extern const char STR_REG_ID[];
extern const char STR_OWNER_ID[];
extern const char STR_HOLD[];
extern const char STR_HOLD_UPPERCASE[];
extern const char STR_NONE[];
extern const char STR_NONE_UPPERCASE[];
extern const char STR_MENUSENSOR[];
extern const char STR_POWERMETER_PEAK[];
extern const char STR_POWERMETER_POWER[];
extern const char STR_POWERMETER_ATTN[];
extern const char STR_POWERMETER_FREQ[];
extern const char STR_MENUTOOLS[];
extern const char STR_TURN_OFF_RECEIVER          [];
extern const char STR_STOPPING              [];
extern const char STR_MENU_SPECTRUM_ANALYSER[];
extern const char STR_MENU_POWER_METER[];
extern const char STR_SENSOR[];
extern const char STR_COUNTRYCODE[];
extern const char STR_USBMODE[];
extern const char STR_JACKMODE[];
extern const char STR_DISABLE_INTERNAL[];
extern const char STR_BATT_CALIB[];
extern const char STR_VOLTAGE[];
extern const char STR_CURRENT_CALIB[];
extern const char STR_UNITSSYSTEM[];
extern const char STR_VOICELANG[];
extern const char STR_MODELIDUSED[];
extern const char STR_BEEP_VOLUME[];
extern const char STR_WAV_VOLUME[];
extern const char STR_BG_VOLUME[];
extern const char STR_SELECT_MODEL[];
extern const char STR_CREATE_CATEGORY[];
extern const char STR_RENAME_CATEGORY[];
extern const char STR_DELETE_CATEGORY[];
extern const char STR_CREATE_MODEL[];
extern const char STR_DUPLICATE_MODEL[];
extern const char STR_COPY_MODEL[];
extern const char STR_MOVE_MODEL[];
extern const char STR_DELETE_MODEL[];
extern const char STR_EDIT[];
extern const char STR_INSERT_BEFORE[];
extern const char STR_INSERT_AFTER[];
extern const char STR_COPY[];
extern const char STR_MOVE[];
extern const char STR_PASTE[];
extern const char STR_INSERT[];
extern const char STR_DELETE[];
extern const char STR_RESET_FLIGHT[];
extern const char STR_RESET_TIMER1[];
extern const char STR_RESET_TIMER2[];
extern const char STR_RESET_TIMER3[];
extern const char STR_RESET_TELEMETRY[];
extern const char STR_STATISTICS[];
extern const char STR_ABOUT_US[];
extern const char STR_USB_JOYSTICK[];
extern const char STR_USB_MASS_STORAGE[];
extern const char STR_USB_SERIAL[];
extern const char STR_SETUP_SCREENS[];
extern const char STR_MONITOR_SCREENS[];
extern const char STR_RESET_BTN[];
extern const char STR_DEBUG[];
extern const char STR_KEYS_BTN[];
extern const char STR_ANALOGS_BTN[];
extern const char STR_CALIBRATION[];
extern const char STR_NO_TOOLS[];
extern const char STR_NORMAL[];
extern const char STR_NOT_INVERTED[];
extern const char STR_NOT_CONNECTED[];
extern const char STR_CONNECTED[];
extern const char STR_FLEX_915[];
extern const char STR_FLEX_868[];
extern const char STR_16CH_WITHOUT_TELEMETRY[];
extern const char STR_16CH_WITH_TELEMETRY[];
extern const char STR_8CH_WITH_TELEMETRY[];
extern const char STR_EXT_ANTENNA[];
extern const char STR_PIN[];
extern const char STR_UPDATE_RX_OPTIONS[];
extern const char STR_UPDATE_TX_OPTIONS[];
extern const char STR_MENU_MODULES_RX_VERSION[];
extern const char STR_GYRO[];
extern const char STR_STICKS_POTS_SLIDERS[];
extern const char STR_PWM_STICKS_POTS_SLIDERS[];
extern const char STR_RF_PROTOCOL[];
extern const char STR_MODULE_OPTIONS[];
extern const char STR_POWER[];
extern const char STR_NO_TX_OPTIONS[];
extern const char STR_RTC_BATT[];
extern const char STR_POWER_METER_EXT[];
extern const char STR_POWER_METER_INT[];
extern const char STR_SPECTRUM_ANALYSER_EXT[];
extern const char STR_SPECTRUM_ANALYSER_INT[];
extern const char STR_WAITING_FOR_RX[];
extern const char STR_WAITING_FOR_TX[];

#if defined(SDCARD)
  extern const char STR_BACKUP_MODEL[];
  extern const char STR_RESTORE_MODEL[];
  extern const char STR_DELETE_ERROR[];
  extern const char STR_SDCARD_ERROR[];
  extern const char STR_NO_SDCARD[];
  extern const char STR_SDCARD_FULL[];
  extern const char STR_INCOMPATIBLE[];
  extern const char STR_LOGS_PATH[];
  extern const char STR_LOGS_EXT[];
  extern const char STR_MODELS_PATH[];
  extern const char STR_MODELS_EXT[];
  #define STR_UPDATE_LIST STR_DELAYDOWN
#endif

extern const char STR_CAT_NOT_EMPTY[];
extern const char STR_WARNING[];
extern const char STR_STORAGE_WARNING[];
extern const char STR_EEPROM_CONVERTING[];
extern const char STR_THROTTLEWARN[];
extern const char STR_ALARMSWARN[];
extern const char STR_SWITCHWARN[];
extern const char STR_FAILSAFEWARN[];
extern const char STR_NIGHTLY_WARNING[];
extern const char STR_NIGHTLY_NOTSAFE[];
extern const char STR_WRONG_SDCARDVERSION[];
extern const char STR_WRONG_PCBREV[];
extern const char STR_EMERGENCY_MODE[];
extern const char STR_NO_FAILSAFE[];
extern const char STR_KEYSTUCK[];

extern const char STR_SPEAKER_VOLUME[];
extern const char STR_LCD[];
extern const char STR_BRIGHTNESS[];
extern const char STR_CPU_TEMP[];
extern const char STR_CPU_CURRENT[];
extern const char STR_CPU_MAH[];
extern const char STR_COPROC[];
extern const char STR_COPROC_TEMP[];
extern const char STR_CAPAWARNING[];
extern const char STR_TEMPWARNING[];
extern const char STR_FUNC[];
extern const char STR_V1[];
extern const char STR_V2[];
extern const char STR_DURATION[];
extern const char STR_DELAY[];
extern const char STR_SD_CARD[];
extern const char STR_SDHC_CARD[];
extern const char STR_NO_SOUNDS_ON_SD[];
extern const char STR_NO_MODELS_ON_SD[];
extern const char STR_NO_BITMAPS_ON_SD[];
extern const char STR_NO_SCRIPTS_ON_SD[];
extern const char STR_SCRIPT_SYNTAX_ERROR[];
extern const char STR_SCRIPT_PANIC[];
extern const char STR_SCRIPT_KILLED[];
extern const char STR_SCRIPT_ERROR[];
extern const char STR_PLAY_FILE[];
extern const char STR_ASSIGN_BITMAP[];
#if defined(PCBHORUS)
extern const char STR_ASSIGN_SPLASH[];
#endif
extern const char STR_EXECUTE_FILE[];
extern const char STR_DELETE_FILE[];
extern const char STR_COPY_FILE[];
extern const char STR_RENAME_FILE[];
extern const char STR_REMOVED[];
extern const char STR_SD_INFO[];
extern const char STR_SD_FORMAT[];
extern const char STR_NA[];
extern const char STR_HARDWARE[];
extern const char STR_FORMATTING[];
extern const char STR_TEMP_CALIB[];
extern const char STR_TIME[];
extern const char STR_MAXBAUDRATE[];
extern const char STR_BAUDRATE[];
extern const char STR_SD_INFO_TITLE[];
extern const char STR_SD_TYPE[];
extern const char STR_SD_SPEED[];
extern const char STR_SD_SECTORS[];
extern const char STR_SD_SIZE[];
extern const char STR_TYPE[];
extern const char STR_GLOBAL_VARS[];
extern const char STR_GVARS[];
extern const char STR_GLOBAL_VAR[];
extern const char STR_OWN[];
extern const char STR_DATE[];
extern const char STR_ROTARY_ENCODER[];
extern const char STR_CHANNELS_MONITOR[];
extern const char STR_MIXERS_MONITOR[];
extern const char STR_PATH_TOO_LONG[];
extern const char STR_VIEW_TEXT[];
extern const char STR_FLASH_BOOTLOADER[];
extern const char STR_FLASH_EXTERNAL_DEVICE[];
extern const char STR_FLASH_RECEIVER_OTA[];
extern const char STR_FLASH_BLUETOOTH_MODULE[];
extern const char STR_FLASH_POWER_MANAGEMENT_UNIT[];
extern const char STR_CURRENT_VERSION[];
extern const char STR_FLASH_INTERNAL_MODULE[];
extern const char STR_FLASH_EXTERNAL_MODULE[];
extern const char STR_FIRMWARE_UPDATE_ERROR[];
extern const char STR_FIRMWARE_UPDATE_SUCCESS[];
extern const char STR_WRITING[];
extern const char STR_CONFIRM_FORMAT[];
extern const char STR_CONFIRMRESET[];
extern const char STR_TOO_MANY_LUA_SCRIPTS[];
extern const char STR_BLCOLOR[];

struct LanguagePack {
  const char * id;
  const char * name;
  void (*playNumber)(getvalue_t number, uint8_t unit, uint8_t flags, uint8_t id);
  void (*playDuration)(int seconds, uint8_t flags, uint8_t id);
};

extern const LanguagePack * currentLanguagePack;
extern uint8_t currentLanguagePackIdx;

extern const LanguagePack czLanguagePack;
extern const LanguagePack deLanguagePack;
extern const LanguagePack enLanguagePack;
extern const LanguagePack esLanguagePack;
extern const LanguagePack frLanguagePack;
extern const LanguagePack huLanguagePack;
extern const LanguagePack itLanguagePack;
extern const LanguagePack nlLanguagePack;
extern const LanguagePack plLanguagePack;
extern const LanguagePack ptLanguagePack;
extern const LanguagePack ruLanguagePack;
extern const LanguagePack seLanguagePack;
extern const LanguagePack skLanguagePack;
extern const LanguagePack * const languagePacks[];

#if defined(LANGUAGE_PACKS_DEFINITION)
const LanguagePack * const languagePacks[] = {
  // alphabetical order
  &czLanguagePack,
  &deLanguagePack,
  &enLanguagePack,
  &esLanguagePack,
  &frLanguagePack,
  &huLanguagePack,
  &itLanguagePack,
  &nlLanguagePack,
  &plLanguagePack,
  &ptLanguagePack,
  &ruLanguagePack,
  &seLanguagePack,
  &skLanguagePack,
  NULL
};
#endif

#if defined(SIMU)
  #define LANGUAGE_PACK_DECLARE(lng, name) const LanguagePack lng ## LanguagePack = { #lng, name, lng ## _ ## playNumber, lng ## _ ## playDuration }
#else
  #define LANGUAGE_PACK_DECLARE(lng, name) extern const LanguagePack lng ## LanguagePack = { #lng, name, lng ## _ ## playNumber, lng ## _ ## playDuration }
#endif

#define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name) LANGUAGE_PACK_DECLARE(lng, name); const LanguagePack * currentLanguagePack = & lng ## LanguagePack; uint8_t currentLanguagePackIdx
inline PLAY_FUNCTION(playNumber, getvalue_t number, uint8_t unit, uint8_t flags) { currentLanguagePack->playNumber(number, unit, flags, id); }
inline PLAY_FUNCTION(playDuration, int seconds, uint8_t flags) { currentLanguagePack->playDuration(seconds, flags, id); }

extern const char STR_MODELNAME[];
extern const char STR_PHASENAME[];
extern const char STR_MIXNAME[];
extern const char STR_INPUTNAME[];
extern const char STR_EXPONAME[];

#if defined(COLORLCD)
  #define TR_PHASES_HEADERS { TR_PHASES_HEADERS_NAME, TR_PHASES_HEADERS_SW, TR_PHASES_HEADERS_RUD_TRIM, TR_PHASES_HEADERS_ELE_TRIM, TR_PHASES_HEADERS_THT_TRIM, TR_PHASES_HEADERS_AIL_TRIM, TR_PHASES_HEADERS_CH5_TRIM, TR_PHASES_HEADERS_CH6_TRIM, TR_PHASES_HEADERS_FAD_IN, TR_PHASES_HEADERS_FAD_OUT }
#elif defined(PCBTARANIS)
  #define TR_PHASES_HEADERS { HINT(TR_PHASES_HEADERS_NAME), HINT(TR_PHASES_HEADERS_SW), HINT(TR_PHASES_HEADERS_RUD_TRIM), HINT(TR_PHASES_HEADERS_ELE_TRIM), HINT(TR_PHASES_HEADERS_THT_TRIM), HINT(TR_PHASES_HEADERS_AIL_TRIM), HINT(TR_PHASES_HEADERS_FAD_IN), HINT(TR_PHASES_HEADERS_FAD_OUT) }
#endif

#define TR_LIMITS_HEADERS { HINT(TR_LIMITS_HEADERS_NAME), HINT(TR_LIMITS_HEADERS_SUBTRIM), HINT(TR_LIMITS_HEADERS_MIN), HINT(TR_LIMITS_HEADERS_MAX), HINT(TR_LIMITS_HEADERS_DIRECTION), HINT(TR_LIMITS_HEADERS_CURVE), HINT(TR_LIMITS_HEADERS_PPMCENTER), HINT(TR_LIMITS_HEADERS_SUBTRIMMODE) }
#define TR_LSW_HEADERS    { HINT(TR_LSW_HEADERS_FUNCTION), HINT(TR_LSW_HEADERS_V1), HINT(TR_LSW_HEADERS_V2), HINT(TR_LSW_HEADERS_V2), HINT(TR_LSW_HEADERS_ANDSW), HINT(TR_LSW_HEADERS_DURATION), HINT(TR_LSW_HEADERS_DELAY) }
#define TR_GVAR_HEADERS   { HINT(TR_GVAR_HEADERS_FM0), HINT(TR_GVAR_HEADERS_FM1), HINT(TR_GVAR_HEADERS_FM2), HINT(TR_GVAR_HEADERS_FM3), HINT(TR_GVAR_HEADERS_FM4), HINT(TR_GVAR_HEADERS_FM5), HINT(TR_GVAR_HEADERS_FM6), HINT(TR_GVAR_HEADERS_FM7), HINT(TR_GVAR_HEADERS_FM8) }

#if LCD_W >= 212
  extern const char * const STR_PHASES_HEADERS[];
  extern const char * const STR_LIMITS_HEADERS[];
  extern const char * const STR_LSW_HEADERS[];
  extern const char * const STR_LSW_DESCRIPTIONS[];
  extern const char * const STR_GVAR_HEADERS[];
#endif

  extern const char STR_TRAINER[];
extern const char STR_MODULE_BIND[];
extern const char STR_POWERMETER_ATTN_NEEDED[];
extern const char STR_PXX2_SELECT_RX[];
extern const char STR_PXX2_DEFAULT[];
extern const char STR_BT_SELECT_DEVICE[];
extern const char STR_DISCOVER[];
extern const char STR_BUTTON_INIT[];
extern const char STR_WAITING[];
extern const char STR_RECEIVER_DELETE[];
extern const char STR_RECEIVER_RESET[];
extern const char STR_SHARE[];
extern const char STR_BIND[];
extern const char STR_REGISTER[];
extern const char STR_BINDING_1_8_TELEM_ON[];
extern const char STR_BINDING_1_8_TELEM_OFF[];
extern const char STR_BINDING_9_16_TELEM_ON[];
extern const char STR_BINDING_9_16_TELEM_OFF[];
extern const char STR_CHANNELRANGE[];
extern const char STR_ANTENNASELECTION[];
extern const char STR_ANTENNACONFIRM1[];
extern const char STR_ANTENNACONFIRM2[];
extern const char STR_R9M_PROTO_FLEX_WARN_LINE1[];
extern const char STR_R9M_PROTO_FCC_WARN_LINE1[];
extern const char STR_R9M_PROTO_EU_WARN_LINE1[];
extern const char STR_R9M_PROTO_WARN_LINE2[];
extern const char STR_SET[];
extern const char STR_PREFLIGHT[];
extern const char STR_CHECKLIST[];
extern const char STR_VIEW_NOTES[];
extern const char STR_MODEL_SELECT[];
extern const char STR_RESET_SUBMENU[];
extern const char STR_LOWALARM[];
extern const char STR_CRITICALALARM[];
extern const char STR_RSSIALARM_WARN[];
extern const char STR_NO_RSSIALARM[];
extern const char STR_DISABLE_ALARM[];
extern const char STR_TELEMETRY_TYPE[];
extern const char STR_TELEMETRY_SENSORS[];
extern const char STR_VALUE[];
extern const char STR_TOPLCDTIMER[];
extern const char STR_UNIT[] ;
extern const char STR_TELEMETRY_NEWSENSOR[];
extern const char STR_ID[];
extern const char STR_PRECISION[];
extern const char STR_RATIO[];
extern const char STR_FORMULA[];
extern const char STR_CELLINDEX[];
extern const char STR_LOGS[];
extern const char STR_OPTIONS[];
extern const char STR_ALTSENSOR[];
extern const char STR_CELLSENSOR[];
extern const char STR_GPSSENSOR[];
extern const char STR_CURRENTSENSOR[];
extern const char STR_AUTOOFFSET[];
extern const char STR_ONLYPOSITIVE[];
extern const char STR_FILTER[];
extern const char STR_TELEMETRYFULL[];
extern const char STR_INVERTED_SERIAL[];
extern const char STR_IGNORE_INSTANCE[];
extern const char STR_DISCOVER_SENSORS[];
extern const char STR_STOP_DISCOVER_SENSORS[];
extern const char STR_DELETE_ALL_SENSORS[];
extern const char STR_CONFIRMDELETE[];
extern const char STR_SELECT_WIDGET[];
extern const char STR_REMOVE_WIDGET[];
extern const char STR_WIDGET_SETTINGS[];
extern const char STR_REMOVE_SCREEN[];
extern const char STR_SETUP_WIDGETS[];
extern const char STR_USER_INTERFACE[];
extern const char STR_THEME[];
extern const char STR_SETUP[];
extern const char STR_LAYOUT[];
extern const char STR_ADDMAINVIEW[];
extern const char STR_BACKGROUND_COLOR[];
extern const char STR_MAIN_COLOR[];
extern const char STR_TEXT_VIEWER[];
extern const char STR_MULTI_RFPOWER[];

extern const char STR_BYTES[];
extern const char STR_ANTENNAPROBLEM[];
extern const char STR_MODULE[];
extern const char STR_RX_NAME[];
extern const char STR_POPUP[];
extern const char STR_MIN[];
extern const char STR_MAX[];
extern const char STR_CURVE_PRESET[];
extern const char STR_PRESET[];
extern const char STR_MIRROR[];
extern const char STR_CLEAR[];
extern const char STR_RESET[];
extern const char STR_COUNT[];
extern const char STR_PT[];
extern const char STR_PTS[];
extern const char STR_SMOOTH[];
extern const char STR_COPY_STICKS_TO_OFS[];
extern const char STR_COPY_TRIMS_TO_OFS[];
extern const char STR_INCDEC[];
extern const char STR_GLOBALVAR[];
extern const char STR_MIXSOURCE[];
extern const char STR_CONSTANT[];
extern const char STR_TOP_BAR[];
extern const char STR_FLASH_ERASE[];
extern const char STR_FLASH_WRITE[];
extern const char STR_OTA_UPDATE[];
extern const char STR_MODULE_RESET[];
extern const char STR_UNKNOWN_RX[];
extern const char STR_UNSUPPORTED_RX[];
extern const char STR_OTA_UPDATE_ERROR[];
extern const char STR_DEVICE_RESET[];
extern const char STR_ALTITUDE[];
extern const char STR_SCALE[];
extern const char STR_VIEW_CHANNELS[];
extern const char STR_POTWARNING[];
extern const char STR_AUX_SERIAL_MODE[];
extern const char STR_THROTTLE_LABEL[];
extern const char STR_SCRIPT[];
extern const char STR_INPUTS[];
extern const char STR_OUTPUTS[];
extern const char STR_MENU_INPUTS[];
extern const char STR_MENU_LUA[];
extern const char STR_MENU_STICKS[];
extern const char STR_MENU_POTS[];
extern const char STR_MENU_MAX[];
extern const char STR_MENU_HELI[];
extern const char STR_MENU_TRIMS[];
extern const char STR_MENU_SWITCHES[];
extern const char STR_MENU_LOGICAL_SWITCHES[];
extern const char STR_MENU_TRAINER[];
extern const char STR_MENU_CHANNELS[];
extern const char STR_MENU_GVARS[];
extern const char STR_MENU_TELEMETRY[];
extern const char STR_MENU_DISPLAY[];
extern const char STR_MENU_OTHER[];
extern const char STR_MENU_INVERT[];
extern const char STR_JITTER_FILTER[];

#if MENUS_LOCK == 1
  extern const char STR_UNLOCKED[];
  extern const char STR_MODS_FORBIDDEN[];
#endif

extern const char STR_MODULE_RANGE[];
extern const char STR_RECEIVER_OPTIONS[];

extern const char STR_ABOUTUS[];
extern const char STR_ABOUT_OPENTX_1[];
extern const char STR_ABOUT_OPENTX_2[];
extern const char STR_ABOUT_OPENTX_3[];
extern const char STR_ABOUT_OPENTX_4[];
extern const char STR_ABOUT_OPENTX_5[];

extern const char STR_ABOUT_BERTRAND_1[];
extern const char STR_ABOUT_BERTRAND_2[];
extern const char STR_ABOUT_BERTRAND_3[];

extern const char STR_ABOUT_MIKE_1[];
extern const char STR_ABOUT_MIKE_2[];
extern const char STR_ABOUT_MIKE_3[];
extern const char STR_ABOUT_MIKE_4[];

extern const char STR_ABOUT_ROMOLO_1[];
extern const char STR_ABOUT_ROMOLO_2[];
extern const char STR_ABOUT_ROMOLO_3[];

extern const char STR_ABOUT_ANDRE_1[];
extern const char STR_ABOUT_ANDRE_2[];
extern const char STR_ABOUT_ANDRE_3[];

extern const char STR_ABOUT_ROB_1[];
extern const char STR_ABOUT_ROB_2[];

extern const char STR_ABOUT_MARTIN_1[];
extern const char STR_ABOUT_MARTIN_2[];

extern const char STR_ABOUT_KJELL_1[];
extern const char STR_ABOUT_KJELL_2[];
extern const char STR_ABOUT_KJELL_3[];
extern const char STR_ABOUT_KJELL_4[];

extern const char STR_ABOUT_HARDWARE_1[];
extern const char STR_ABOUT_HARDWARE_2[];
extern const char STR_ABOUT_HARDWARE_3[];

extern const char STR_ABOUT_PARENTS_1[];
extern const char STR_ABOUT_PARENTS_2[];
extern const char STR_ABOUT_PARENTS_3[];
extern const char STR_ABOUT_PARENTS_4[];

#define CHR_HOUR   TR_CHR_HOUR
#define CHR_INPUT  TR_CHR_INPUT

#endif // _TRANSLATIONS_H_
