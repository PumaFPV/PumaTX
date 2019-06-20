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

#ifndef _OPENTX_H_
#define _OPENTX_H_

#include <inttypes.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "definitions.h"
#include "opentx_types.h"
#include "debounce.h"

#if defined(STM32)
#include "usbd_conf.h"
#endif

#if defined(SIMU)
  #define SWITCH_SIMU(a, b)  (a)
#else
  #define SWITCH_SIMU(a, b)  (b)
#endif

#if defined(PCBSKY9X)
  #define IS_PCBSKY9X        true
  #define CASE_PCBSKY9X(x)   x,
#else
  #define IS_PCBSKY9X        false
  #define CASE_PCBSKY9X(x)
#endif

#if defined(STM32)
  #define CASE_STM32(x)     x,
#else
  #define CASE_STM32(x)
#endif

#if defined(VARIO)
  #define CASE_VARIO(x) x,
#else
  #define CASE_VARIO(x)
#endif

#if defined(GYRO)
#define CASE_GYRO(x) x,
#else
#define CASE_GYRO(x)
#endif

#if defined(LUA)
  #define CASE_LUA(x) x,
#else
  #define CASE_LUA(x)
#endif

#if defined(RTCLOCK)
  #define CASE_RTCLOCK(x) x,
#else
  #define CASE_RTCLOCK(x)
#endif

#if defined(BUZZER)
  #define CASE_BUZZER(x) x,
#else
  #define CASE_BUZZER(x)
#endif

#if defined(AUDIO)
  #define CASE_AUDIO(x) x,
#else
  #define CASE_AUDIO(x)
#endif

#if defined(PWM_BACKLIGHT)
  #define CASE_PWM_BACKLIGHT(x) x,
#else
  #define CASE_PWM_BACKLIGHT(x)
#endif

#if defined(TELEMETRY_FRSKY) && defined(GPS)
  #define CASE_GPS(x) x,
#else
  #define CASE_GPS(x)
#endif

#if defined(VARIO)
  #define CASE_VARIO(x) x,
#else
  #define CASE_VARIO(x)
#endif

#if defined(HAPTIC)
  #define CASE_HAPTIC(x) x,
#else
  #define CASE_HAPTIC(x)
#endif

#if defined(SPLASH)
  #define CASE_SPLASH(x) x,
#else
  #define CASE_SPLASH(x)
#endif

#if defined(TELEMETRY_FRSKY)
  #define CASE_FRSKY(x) x,
#else
  #define CASE_FRSKY(x)
#endif

#if defined(PXX1)
  #define CASE_PXX1(x) x,
#else
  #define CASE_PXX1(x)
#endif

#if defined(PXX2)
  #define CASE_PXX2(x) x,
#else
  #define CASE_PXX2(x)
#endif

#if defined(SDCARD)
  #define CASE_SDCARD(x) x,
#else
  #define CASE_SDCARD(x)
#endif

#if defined(BLUETOOTH)
  #define CASE_BLUETOOTH(x) x,
#else
  #define CASE_BLUETOOTH(x)
#endif

#if defined(HELI)
  #define CASE_HELI(x) x,
#else
  #define CASE_HELI(x)
#endif

#if defined(FLIGHT_MODES)
  #define CASE_FLIGHT_MODES(x) x,
#else
  #define CASE_FLIGHT_MODES(x)
#endif

#if defined(GVARS)
  #define CASE_GVARS(x) x,
#else
  #define CASE_GVARS(x)
#endif

#if defined(PCBX9DP) || defined(PCBX9E)
  #define CASE_PCBX9E_PCBX9DP(x) x,
#else
  #define CASE_PCBX9E_PCBX9DP(x)
#endif

#if defined(PCBX9E)
  #define CASE_PCBX9E(x) x,
#else
  #define CASE_PCBX9E(x)
#endif

#if defined(PCBSKY9X) && !defined(PCBAR9X)
  #define TX_CAPACITY_MEASUREMENT
  #define CASE_CAPACITY(x) x,
#else
  #define CASE_CAPACITY(x)
#endif

#if defined(FAI)
  #define IS_FAI_ENABLED() true
  #define IF_FAI_CHOICE(x)
#elif defined(FAI_CHOICE)
  #define IS_FAI_ENABLED() g_eeGeneral.fai
  #define IF_FAI_CHOICE(x) x,
#else
  #define IS_FAI_ENABLED() false
  #define IF_FAI_CHOICE(x)
#endif

#define IS_FAI_FORBIDDEN(idx) (IS_FAI_ENABLED() && isFaiForbidden(idx))

#if defined(BLUETOOTH)
  #if defined(X9E)
    #define IS_BLUETOOTH_TRAINER()       (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH)
    #define IS_SLAVE_TRAINER()           (g_model.trainerData.mode == TRAINER_MODE_SLAVE)
  #else
    #define IS_BLUETOOTH_TRAINER()       (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH || g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH)
    #define IS_SLAVE_TRAINER()           (g_model.trainerData.mode == TRAINER_MODE_SLAVE || g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH)
  #endif
#else
  #define IS_BLUETOOTH_TRAINER()       false
  #define IS_SLAVE_TRAINER()           (g_model.trainerData.mode == TRAINER_MODE_SLAVE)
#endif

// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l

#include "board.h"

#if defined(JACK_DETECT_GPIO)
#define CASE_JACK_DETECT(x) x,
#else
#define CASE_JACK_DETECT(x)
#endif

#if defined(DISK_CACHE)
  #include "disk_cache.h"
#endif

#if defined(SIMU)
  #include "targets/simu/simpgmspace.h"
#endif

#include "debug.h"

#if defined(PCBTARANIS) || defined(PCBHORUS)
  #define SWSRC_THR                    SWSRC_SB2
  #define SWSRC_GEA                    SWSRC_SG2
  #define SWSRC_ID0                    SWSRC_SA0
  #define SWSRC_ID1                    SWSRC_SA1
  #define SWSRC_ID2                    SWSRC_SA2
  #define IS_MOMENTARY(sw)             false // TODO
#else
  #define SW_DSM2_BIND     SW_TRN
#endif

#include "myeeprom.h"

inline void memclear(void * p, size_t size)
{
  memset(p, 0, size);
}

inline bool is_memclear(void * p, size_t size)
{
  uint8_t * buf = (uint8_t *)p;
  return buf[0] == 0 && !memcmp(buf, buf + 1, size - 1);
}

void memswap(void * a, void * b, uint8_t size);

#if defined(PCBX9D) || defined(PCBX9DP) || defined(PCBX9E) || defined(PCBHORUS)
  #define POT_CONFIG(x)                ((g_eeGeneral.potsConfig >> (2*((x)-POT1)))&0x03)
  #define IS_POT_MULTIPOS(x)           (IS_POT(x) && POT_CONFIG(x)==POT_MULTIPOS_SWITCH)
  #define IS_POT_WITHOUT_DETENT(x)     (IS_POT(x) && POT_CONFIG(x)==POT_WITHOUT_DETENT)
  #define IS_SLIDER_AVAILABLE(x)       ((x) == SLIDER1 || (x) == SLIDER2 || (IS_SLIDER(x) && (g_eeGeneral.slidersConfig & (0x01 << ((x)-SLIDER1)))))
  #define IS_POT_AVAILABLE(x)          (IS_POT(x) && POT_CONFIG(x)!=POT_NONE)
  #define IS_POT_SLIDER_AVAILABLE(x)   (IS_POT_AVAILABLE(x) || IS_SLIDER_AVAILABLE(x))
  #define IS_MULTIPOS_CALIBRATED(cal)  (cal->count>0 && cal->count<XPOTS_MULTIPOS_COUNT)
#elif defined(PCBX7) || defined(PCBXLITE)
  #define POT_CONFIG(x)                ((g_eeGeneral.potsConfig >> (2*((x)-POT1)))&0x03)
  #define IS_POT_MULTIPOS(x)           (IS_POT(x) && POT_CONFIG(x)==POT_MULTIPOS_SWITCH)
  #define IS_POT_WITHOUT_DETENT(x)     (IS_POT(x) && POT_CONFIG(x)==POT_WITHOUT_DETENT)
  #define IS_POT_AVAILABLE(x)          (IS_POT(x) && POT_CONFIG(x)!=POT_NONE)
  #define IS_POT_SLIDER_AVAILABLE(x)   (IS_POT_AVAILABLE(x))
  #define IS_MULTIPOS_CALIBRATED(cal)  (cal->count>0 && cal->count<XPOTS_MULTIPOS_COUNT)
#else
  #define IS_POT_MULTIPOS(x)           (false)
  #define IS_POT_WITHOUT_DETENT(x)     (true)
  #define IS_POT_SLIDER_AVAILABLE(x)   (true)
  #define IS_MULTIPOS_CALIBRATED(cal)  (false)
#endif

#if defined(PWR_BUTTON_PRESS)
  #define pwrOffPressed()              pwrPressed()
#else
  #define pwrOffPressed()              (!pwrPressed())
#endif

#define PWR_PRESS_SHUTDOWN_DELAY       300 // 3s

#define GET_LOWRES_POT_POSITION(i)     (getValue(MIXSRC_FIRST_POT+(i)) >> 4)
#define SAVE_POT_POSITION(i)           g_model.potsWarnPosition[i] = GET_LOWRES_POT_POSITION(i)

#define PPM_CENTER                     1500

#if defined(PPM_CENTER_ADJUSTABLE)
  #define PPM_CH_CENTER(ch)            (PPM_CENTER + limitAddress(ch)->ppmCenter)
#else
  #define PPM_CH_CENTER(ch)            (PPM_CENTER)
#endif

#include "fifo.h"
#include "io/frsky_sport.h"

#if defined(CLI)
#include "cli.h"
#endif

#include "timers.h"
#include "storage/storage.h"
#include "pulses/pulses.h"
#include "pulses/modules_helpers.h"

#define MASK_CFN_TYPE  uint64_t  // current max = 64 function switches
#define MASK_FUNC_TYPE uint32_t  // current max = 32 functions

typedef struct {
  MASK_FUNC_TYPE activeFunctions;
  MASK_CFN_TYPE  activeSwitches;
  tmr10ms_t lastFunctionTime[MAX_SPECIAL_FUNCTIONS];

  inline bool isFunctionActive(uint8_t func)
  {
    return activeFunctions & ((MASK_FUNC_TYPE)1 << func);
  }

  void reset()
  {
    memclear(this, sizeof(*this));
  }
} CustomFunctionsContext;

#include "strhelpers.h"
#include "gui.h"

#if !defined(SIMU)
  #define assert(x)
  #if !defined(DEBUG)
    #define printf printf_not_allowed
  #endif
#endif

extern const uint8_t bchout_ar[];
extern const uint8_t modn12x3[];

//convert from mode 1 to mode stickMode
//NOTICE!  =>  0..3 -> 0..3
#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3
#define CONVERT_MODE(x)          (((x)<=AIL_STICK) ? *(modn12x3 + 4*g_eeGeneral.stickMode + (x)) : (x) )

#if defined(PCBXLITE)
  #define CONVERT_MODE_TRIMS(x)  (((x) == RUD_STICK) ? AIL_STICK : ((x) == AIL_STICK) ? RUD_STICK : (x))
#else
  #define CONVERT_MODE_TRIMS(x)  CONVERT_MODE(x)
#endif

extern uint8_t channel_order(uint8_t x);

#define THRCHK_DEADBAND                16

#if defined(COLORLCD)
  #define SPLASH_NEEDED()              (false)
#elif defined(PCBTARANIS)
  #define SPLASH_NEEDED()              (g_eeGeneral.splashMode != 3)
#else
  #define SPLASH_NEEDED()              (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_DSM2 && !g_eeGeneral.splashMode)
#endif

#if defined(PCBHORUS)
  #define SPLASH_TIMEOUT               0 /* we use the splash duration to load stuff from the SD */
#elif defined(PCBTARANIS)
  #define SPLASH_TIMEOUT               (g_eeGeneral.splashMode==-4 ? 1500 : (g_eeGeneral.splashMode<=0 ? (400-g_eeGeneral.splashMode*200) : (400-g_eeGeneral.splashMode*100)))
#else
  #define SPLASH_TIMEOUT               (4*100)  // 4 seconds
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define IS_ROTARY_ENCODER_NAVIGATION_ENABLE()  true
  extern volatile rotenc_t rotencValue;
  #define ROTARY_ENCODER_NAVIGATION_VALUE        rotencValue
  extern uint8_t rotencSpeed;
  #define ROTENC_LOWSPEED              1
  #define ROTENC_MIDSPEED              5
  #define ROTENC_HIGHSPEED             50
  #define ROTENC_DELAY_MIDSPEED        32
  #define ROTENC_DELAY_HIGHSPEED       16
#endif

constexpr uint8_t HEART_TIMER_10MS = 0x01;
constexpr uint8_t HEART_TIMER_PULSES = 0x02; // when multiple modules this is the first one
#if defined(PCBTARANIS) || defined(PCBHORUS)
constexpr uint8_t HEART_WDT_CHECK = (HEART_TIMER_10MS + (HEART_TIMER_PULSES << 0) + (HEART_TIMER_PULSES << 1));
#else
constexpr uint8_t HEART_WDT_CHECK = (HEART_TIMER_10MS + HEART_TIMER_PULSES);
#endif
extern uint8_t heartbeat;

#if !defined(BOOT)
void watchdogSuspend(uint32_t timeout);
#define WATCHDOG_SUSPEND(x)            watchdogSuspend(x)
#else
#define WATCHDOG_SUSPEND(...)
#endif

#define MAX_ALERT_TIME   60

struct t_inactivity
{
  uint16_t counter;
  uint8_t  sum;
};

extern struct t_inactivity inactivity;

#define LEN_STD_CHARS 40

#if defined(TRANSLATIONS_CZ)
#define ZCHAR_MAX (LEN_STD_CHARS)
#else
#define ZCHAR_MAX (LEN_STD_CHARS + LEN_SPECIAL_CHARS)
#endif

char hex2zchar(uint8_t hex);
char zchar2char(int8_t idx);
char char2lower(char c);
int8_t char2zchar(char c);
void str2zchar(char *dest, const char *src, int size);
int zchar2str(char *dest, const char *src, int size);
bool cmpStrWithZchar(const char * charString, const char * zcharString, int size);

#include "keys.h"
#include "pwr.h"

#if defined(PCBTARANIS) || defined(PCBHORUS)
div_t switchInfo(int switchPosition);
extern uint8_t potsPos[NUM_XPOTS];
#endif

bool trimDown(uint8_t idx);
void readKeysAndTrims();

uint16_t evalChkSum();

#if !defined(GUI)
  #define RAISE_ALERT(...)
  #define ALERT(...)
#else
  #define RAISE_ALERT(title, msg, info, sound) showAlertBox(title, msg, info, sound)
  #define ALERT(title, msg, sound) alert(title, msg, sound)
#endif

void alert(const char * t, const char * s , uint8_t sound);

enum PerOutMode {
  e_perout_mode_normal = 0,
  e_perout_mode_inactive_flight_mode = 1,
  e_perout_mode_notrainer = 2,
  e_perout_mode_notrims = 4,
  e_perout_mode_nosticks = 8,
  e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};

extern uint8_t mixerCurrentFlightMode;
extern uint8_t lastFlightMode;
extern uint8_t flightModeTransitionLast;

#if defined(SIMU)
  inline int availableMemory() { return 1000; }
#elif !defined(SIMU)
  extern unsigned char *heap;
  extern int _end;
  extern int _heap_end;
  #define availableMemory() ((unsigned int)((unsigned char *)&_heap_end - heap))
#endif

void evalFlightModeMixes(uint8_t mode, uint8_t tick10ms);
void evalMixes(uint8_t tick10ms);
void doMixerCalculations();
void scheduleNextMixerCalculation(uint8_t module, uint16_t period_ms);

void checkTrims();
void perMain();
void per10ms();

getvalue_t getValue(mixsrc_t i);

#define GETSWITCH_MIDPOS_DELAY   1
bool getSwitch(swsrc_t swtch, uint8_t flags=0);

void logicalSwitchesTimerTick();
void logicalSwitchesReset();

void evalLogicalSwitches(bool isCurrentFlightmode=true);
void logicalSwitchesCopyState(uint8_t src, uint8_t dst);
#define LS_RECURSIVE_EVALUATION_RESET()

#if defined(PCBTARANIS) || defined(PCBHORUS)
  void getSwitchesPosition(bool startup);
#else
  #define getSwitchesPosition(...)
#endif

extern swarnstate_t switches_states;
swsrc_t getMovedSwitch();

#define GET_MOVED_SOURCE_PARAMS uint8_t min
int8_t getMovedSource(GET_MOVED_SOURCE_PARAMS);
#define GET_MOVED_SOURCE(min, max) getMovedSource(min)

#if defined(FLIGHT_MODES)
  extern uint8_t getFlightMode();
#else
  #define getFlightMode() 0
#endif

#define getTrimFlightMode(phase, idx) (phase)

#if defined(GVARS)
  extern int8_t trimGvar[NUM_TRIMS];
  #define TRIM_REUSED(idx) trimGvar[idx] >= 0
#else
  #define TRIM_REUSED(idx) 0
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx);
int getTrimValue(uint8_t phase, uint8_t idx);

bool setTrimValue(uint8_t phase, uint8_t idx, int trim);

#if defined(PCBSKY9X)
  #define ROTARY_ENCODER_GRANULARITY (2 << g_eeGeneral.rotarySteps)
#else
  #define ROTARY_ENCODER_GRANULARITY (2)
#endif

#include "gvars.h"

extern uint16_t sessionTimer;
extern uint16_t s_timeCumThr;
extern uint16_t s_timeCum16ThrP;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
#define OVERRIDE_CHANNEL_UNDEFINED -4096
extern safetych_t safetyCh[MAX_OUTPUT_CHANNELS];
#endif

extern uint8_t trimsCheckTimer;
extern uint8_t trimsDisplayTimer;
extern uint8_t trimsDisplayMask;

void flightReset(uint8_t check=true);

extern uint8_t unexpectedShutdown;

extern uint16_t vbattRTC;

extern uint16_t maxMixerDuration;

#define DURATION_MS_PREC2(x) ((x)/20)

#if defined(THRTRACE)
  #if defined(COLORLCD)
    #define MAXTRACE (LCD_W-2*10)
  #else
    #define MAXTRACE (LCD_W - 8)
  #endif
  extern uint8_t  s_traceBuf[MAXTRACE];
  extern uint16_t s_traceWr;
  extern uint8_t  s_cnt_10s;
  extern uint16_t s_cnt_samples_thr_10s;
  extern uint16_t s_sum_samples_thr_10s;
  #define RESET_THR_TRACE() s_traceWr = s_cnt_10s = s_cnt_samples_thr_10s = s_sum_samples_thr_10s = s_timeCum16ThrP = s_timeCumThr = 0
#else
  #define RESET_THR_TRACE() s_timeCum16ThrP = s_timeCumThr = 0
#endif

#if defined(SIMU)
  uint16_t getTmr2MHz();
  uint16_t getTmr16KHz();
#elif defined(STM32)
  static inline uint16_t getTmr2MHz() { return TIMER_2MHz_TIMER->CNT; }
#elif defined(PCBSKY9X)
  static inline uint16_t getTmr2MHz() { return TC1->TC_CHANNEL[0].TC_CV; }
#else
  uint16_t getTmr16KHz();
#endif


#if defined(SPLASH)
  void doSplash();
#endif

#if MENUS_LOCK == 1
  extern bool readonly;
  extern bool readonlyUnlocked();
  #define READ_ONLY() readonly
  #define READ_ONLY_UNLOCKED() readonlyUnlocked()
#else
  #define READ_ONLY() false
  #define READ_ONLY_UNLOCKED() true
#endif

void checkLowEEPROM();
void checkThrottleStick();
void checkSwitches();
void checkAlarm();
void checkAll();

void getADC();
static inline void GET_ADC_IF_MIXER_NOT_RUNNING()
{
  do {
    if (s_pulses_paused) {
      getADC();
    }
  }
  while(0);
}

#include "sbus.h"

void backlightOn();
void checkBacklight();
void doLoopCommonActions();

#define BITMASK(bit) (1<<(bit))

/// returns the number of elements of an array
#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

template<class t> inline t min(t a, t b) { return a<b?a:b; }
template<class t> inline t max(t a, t b) { return a>b?a:b; }
template<class t> inline t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> inline t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }
template<class t> inline void SWAP(t & a, t & b) { t tmp = b; b = a; a = tmp; }

uint16_t isqrt32(uint32_t n);

#if defined(BOOT)
#define pauseMixerCalculations()
#define resumeMixerCalculations()
#else
#include "tasks.h"
extern RTOS_MUTEX_HANDLE mixerMutex;
inline void pauseMixerCalculations()
{
  RTOS_LOCK_MUTEX(mixerMutex);
}

inline void resumeMixerCalculations()
{
  RTOS_UNLOCK_MUTEX(mixerMutex);
}
#endif

void generalDefault();
void modelDefault(uint8_t id);

#if defined(EEPROM)
void checkModelIdUnique(uint8_t index, uint8_t module);
uint8_t findNextUnusedModelId(uint8_t index, uint8_t module);
#endif

uint32_t hash(const void * ptr, uint32_t size);
inline int divRoundClosest(const int n, const int d)
{
  if (d == 0)
    return 0;
  else
    return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}

#define calc100to256_16Bits(x) calc100to256(x)
#define calc100toRESX_16Bits(x) calc100toRESX(x)

inline int calc100to256(int x)
{
  return divRoundClosest(x*256, 100);
}

inline int calc100toRESX(int x)
{
  return divRoundClosest(x*RESX, 100);
}

inline int calc1000toRESX(int x)
{
  return divRoundClosest(x*RESX, 1000);
}

inline int calcRESXto1000(int x)
{
  return divRoundClosest(x*1000, RESX);
}

inline int calcRESXto100(int x)
{
  return divRoundClosest(x*100, RESX);
}


#if defined(COLORLCD)
extern const char vers_stamp[];
extern const char date_stamp[];
extern const char time_stamp[];
extern const char eeprom_stamp[];
#else
extern const char vers_stamp[];
#endif

/**
 * Tries to find opentx version in the first 1024 byte of either firmware/bootloader (the one not running) or the buffer
 * @param buffer If non-null find the firmware version in the buffer instead
 * @return The opentx version string starting with "opentx-" or "no version found" if the version string is not found
 */
const char* getOtherVersion(char* buffer);

extern uint8_t g_vbat100mV;
#if LCD_W > 128
  #define GET_TXBATT_BARS() (limit<int8_t>(0, div_and_round(10 * (g_vbat100mV - g_eeGeneral.vBatMin - 90), 30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 10))
#else
  #define GET_TXBATT_BARS() (limit<int8_t>(2, 20 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 20))
#endif
#define IS_TXBATT_WARNING() (g_vbat100mV <= g_eeGeneral.vBatWarn)


#define g_blinkTmr10ms    (*(uint8_t*)&g_tmr10ms)
extern uint8_t            g_beepCnt;

#include "trainer.h"

extern int32_t            chans[MAX_OUTPUT_CHANNELS];
extern int16_t            ex_chans[MAX_OUTPUT_CHANNELS]; // Outputs (before LIMITS) of the last perMain
extern int16_t            channelOutputs[MAX_OUTPUT_CHANNELS];

int expo(int x, int k);

inline void getMixSrcRange(const int source, int16_t & valMin, int16_t & valMax, LcdFlags * flags = 0)
{
  if (source >= MIXSRC_FIRST_TRIM && source <= MIXSRC_LAST_TRIM) {
    valMax = g_model.extendedTrims ? TRIM_EXTENDED_MAX : TRIM_MAX;
    valMin = -valMax;
  }
#if defined(LUA_INPUTS)
  else if (source >= MIXSRC_FIRST_LUA && source <= MIXSRC_LAST_LUA) {
    valMax = 30000;
    valMin = -valMax;
  }
#endif
  else if (source < MIXSRC_FIRST_CH) {
    valMax = 100;
    valMin = -valMax;
  }
  else if (source <= MIXSRC_LAST_CH) {
    valMax = g_model.extendedLimits ? LIMIT_EXT_PERCENT : 100;
    valMin = -valMax;
  }
#if defined(GVARS)
  else if (source >= MIXSRC_FIRST_GVAR && source <= MIXSRC_LAST_GVAR) {
    valMax = min<int>(CFN_GVAR_CST_MAX, MODEL_GVAR_MAX(source-MIXSRC_FIRST_GVAR));
    valMin = max<int>(CFN_GVAR_CST_MIN, MODEL_GVAR_MIN(source-MIXSRC_FIRST_GVAR));
    if (flags && g_model.gvars[source-MIXSRC_FIRST_GVAR].prec)
      *flags |= PREC1;
  }
#endif
  else if (source == MIXSRC_TX_VOLTAGE) {
    valMax =  255;
    valMin = 0;
    if (flags)
      *flags |= PREC1;
  }
  else if (source == MIXSRC_TX_TIME) {
    valMax =  23 * 60 + 59;
    valMin = 0;
  }
  else if (source >= MIXSRC_FIRST_TIMER && source <= MIXSRC_LAST_TIMER) {
    valMax =  9 * 60 * 60 - 1;
    valMin = -valMax;
    if (flags)
      *flags |= TIMEHOUR;
  }
  else {
    valMax = 30000;
    valMin = -valMax;
  }
}
#if defined(GVAR_MAX)
inline void getGVarIncDecRange(int16_t & valMin, int16_t & valMax)
{
  int16_t rng = abs(valMax - valMin);
  valMin = -rng;
  valMax = rng;
}
#endif

// Curves
enum BaseCurves {
  CURVE_NONE,
  CURVE_X_GT0,
  CURVE_X_LT0,
  CURVE_ABS_X,
  CURVE_F_GT0,
  CURVE_F_LT0,
  CURVE_ABS_F,
  CURVE_BASE
};
int8_t * curveAddress(uint8_t idx);
struct point_t
{
  coord_t x;
  coord_t y;
};
point_t getPoint(uint8_t i);
typedef CurveData CurveInfo;
void loadCurves();
#define LOAD_MODEL_CURVES() loadCurves()
int intpol(int x, uint8_t idx);
int applyCurve(int x, CurveRef & curve);
int applyCustomCurve(int x, uint8_t idx);
int applyCurrentCurve(int x);
int8_t getCurveX(int noPoints, int point);
void resetCustomCurveX(int8_t * points, int noPoints);
bool moveCurve(uint8_t index, int8_t shift); // TODO bool?

void clearInputs();
void defaultInputs();

void applyExpos(int16_t * anas, uint8_t mode, uint8_t ovwrIdx=0, int16_t ovwrValue=0);
int16_t applyLimits(uint8_t channel, int32_t value);

void evalInputs(uint8_t mode);
uint16_t anaIn(uint8_t chan);

extern int16_t calibratedAnalogs[NUM_CALIBRATED_ANALOGS];

#define FLASH_DURATION 20 /*200ms*/

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

FlightModeData * flightModeAddress(uint8_t idx);
ExpoData * expoAddress(uint8_t idx);
MixData * mixAddress(uint8_t idx);
LimitData * limitAddress(uint8_t idx);
LogicalSwitchData * lswAddress(uint8_t idx);

// static variables used in evalFlightModeMixes - moved here so they don't interfere with the stack
// It's also easier to initialize them here.
extern int8_t  virtualInputsTrims[MAX_INPUTS];

extern int16_t anas [MAX_INPUTS];
extern int16_t trims[NUM_TRIMS];
extern BeepANACenter bpanaCenter;

extern uint8_t s_mixer_first_run_done;

void applyDefaultTemplate();

void instantTrim();
void evalTrims();
void copyTrimsToOffset(uint8_t ch);
void copySticksToOffset(uint8_t ch);
void moveTrimsToOffsets();

typedef uint16_t ACTIVE_PHASES_TYPE;
#define DELAY_POS_SHIFT    0
#define DELAY_POS_MARGIN   3
typedef int16_t delayval_t;
PACK(typedef struct {
  uint16_t delay;
  int16_t  now;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  int16_t  prev;
  uint8_t  activeMix;
  uint8_t  activeExpo;
}) SwOn;

extern SwOn   swOn[MAX_MIXERS];
extern int32_t act[MAX_MIXERS];

#if defined(BOLD_FONT)
  inline bool isExpoActive(uint8_t expo)
  {
    return swOn[expo].activeExpo;
  }

  inline bool isMixActive(uint8_t mix)
  {
    return swOn[mix].activeMix;
  }
#else
  #define isExpoActive(x) false
  #define isMixActive(x) false
#endif

enum LogicalSwitchFamilies {
  LS_FAMILY_OFS,
  LS_FAMILY_BOOL,
  LS_FAMILY_COMP,
  LS_FAMILY_DIFF,
  LS_FAMILY_TIMER,
  LS_FAMILY_STICKY,
  LS_FAMILY_RANGE,
  LS_FAMILY_EDGE
};

uint8_t lswFamily(uint8_t func);
int16_t lswTimerValue(delayval_t val);

enum FunctionsActive {
  FUNCTION_TRAINER,
  FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
  FUNCTION_VARIO,
  FUNCTION_BACKLIGHT,
#if defined(SDCARD)
  FUNCTION_LOGS,
#endif
  FUNCTION_BACKGND_MUSIC,
  FUNCTION_BACKGND_MUSIC_PAUSE,
};

#define VARIO_FREQUENCY_ZERO   700/*Hz*/
#define VARIO_FREQUENCY_RANGE  1000/*Hz*/
#define VARIO_REPEAT_ZERO      500/*ms*/
#define VARIO_REPEAT_MAX       80/*ms*/

extern CustomFunctionsContext modelFunctionsContext;
extern CustomFunctionsContext globalFunctionsContext;
inline bool isFunctionActive(uint8_t func)
{
  return globalFunctionsContext.isFunctionActive(func) || modelFunctionsContext.isFunctionActive(func);
}
void evalFunctions(const CustomFunctionData * functions, CustomFunctionsContext & functionsContext);
inline void customFunctionsReset()
{
  globalFunctionsContext.reset();
  modelFunctionsContext.reset();
}

#include "telemetry/telemetry.h"

uint8_t crc8(const uint8_t * ptr, uint32_t len);
uint16_t crc16(const uint8_t * ptr, uint32_t len);

#define PLAY_REPEAT(x)            (x)                 /* Range 0 to 15 */
#define PLAY_NOW                  0x10
#define PLAY_BACKGROUND           0x20

enum AUDIO_SOUNDS {
  AUDIO_HELLO,
  AU_BYE,
  AU_THROTTLE_ALERT,
  AU_SWITCH_ALERT,
  AU_BAD_RADIODATA,
  AU_TX_BATTERY_LOW,
  AU_INACTIVITY,
  AU_RSSI_ORANGE,
  AU_RSSI_RED,
  AU_RAS_RED,
  AU_TELEMETRY_LOST,
  AU_TELEMETRY_BACK,
  AU_TRAINER_LOST,
  AU_TRAINER_BACK,
  AU_SENSOR_LOST,
  AU_SERVO_KO,
  AU_RX_OVERLOAD,
  AU_MODEL_STILL_POWERED,
#if defined(PCBSKY9X)
  AU_TX_MAH_HIGH,
  AU_TX_TEMP_HIGH,
#endif
  AU_ERROR,
  AU_WARNING1,
  AU_WARNING2,
  AU_WARNING3,
  AU_TRIM_MIDDLE,
  AU_TRIM_MIN,
  AU_TRIM_MAX,
  AU_STICK1_MIDDLE,
  AU_STICK2_MIDDLE,
  AU_STICK3_MIDDLE,
  AU_STICK4_MIDDLE,
#if defined(PCBTARANIS) || defined(PCBHORUS)
  AU_POT1_MIDDLE,
  AU_POT2_MIDDLE,
#if defined(PCBX9E)
  AU_POT3_MIDDLE,
  AU_POT4_MIDDLE,
#endif
  AU_SLIDER1_MIDDLE,
  AU_SLIDER2_MIDDLE,
#if defined(PCBX9E)
  AU_SLIDER3_MIDDLE,
  AU_SLIDER4_MIDDLE,
#endif
#else
  AU_POT1_MIDDLE,
  AU_POT2_MIDDLE,
  AU_POT3_MIDDLE,
#endif
  AU_MIX_WARNING_1,
  AU_MIX_WARNING_2,
  AU_MIX_WARNING_3,
  AU_TIMER1_ELAPSED,
  AU_TIMER2_ELAPSED,
  AU_TIMER3_ELAPSED,

  AU_SPECIAL_SOUND_FIRST,
  AU_SPECIAL_SOUND_BEEP1 = AU_SPECIAL_SOUND_FIRST,
  AU_SPECIAL_SOUND_BEEP2,
  AU_SPECIAL_SOUND_BEEP3,
  AU_SPECIAL_SOUND_WARN1,
  AU_SPECIAL_SOUND_WARN2,
  AU_SPECIAL_SOUND_CHEEP,
  AU_SPECIAL_SOUND_RATATA,
  AU_SPECIAL_SOUND_TICK,
  AU_SPECIAL_SOUND_SIREN,
  AU_SPECIAL_SOUND_RING,
  AU_SPECIAL_SOUND_SCIFI,
  AU_SPECIAL_SOUND_ROBOT,
  AU_SPECIAL_SOUND_CHIRP,
  AU_SPECIAL_SOUND_TADA,
  AU_SPECIAL_SOUND_CRICKET,
  AU_SPECIAL_SOUND_ALARMC,
  AU_SPECIAL_SOUND_LAST,

  AU_NONE=0xff
};

#if defined(AUDIO)
#include "audio.h"
#endif

#include "buzzer.h"
#include "translations.h"
#include "fonts.h"

#if defined(HAPTIC)
#include "haptic.h"
#endif

#if defined(SDCARD)
#include "sdcard.h"
#endif

#if defined(RTCLOCK)
#include "rtc.h"
#endif

#if defined(REVX)
void setMFP();
void clearMFP();
#endif

extern uint8_t requiredSpeakerVolume;

enum MainRequest {
  REQUEST_SCREENSHOT,
  REQUEST_FLIGHT_RESET,
};

extern uint8_t mainRequestFlags;

void checkBattery();
void opentxClose(uint8_t shutdown=true);
void opentxInit();
void opentxResume();

constexpr uint8_t OPENTX_START_NO_SPLASH = 0x01;
constexpr uint8_t OPENTX_START_NO_CALIBRATION = 0x02;
constexpr uint8_t OPENTX_START_NO_CHECKS = 0x04;

#if defined(STATUS_LEDS)
  #define LED_ERROR_BEGIN()            ledRed()
  #define LED_ERROR_END()              ledBlue()
#else
  #define LED_ERROR_BEGIN()
  #define LED_ERROR_END()
#endif

// Re-useable byte array to save having multiple buffers
#if LCD_W <= 212
#define SD_SCREEN_FILE_LENGTH          32
#else
#define SD_SCREEN_FILE_LENGTH          64
#endif

#if defined(BLUETOOTH)
#include "bluetooth.h"
#endif

union ReusableBuffer
{
  struct {
#if defined(EEPROM_RLC) && LCD_W < 212
    uint16_t eepromfree;
#endif
#if defined(SDCARD)
    char menu_bss[POPUP_MENU_MAX_LINES][MENU_LINE_LENGTH];
    char mainname[45]; // because reused for SD backup / restore, max backup filename 44 chars: "/MODELS/MODEL0134353-2014-06-19-04-51-27.bin"
#else
    char mainname[LEN_MODEL_NAME];
#endif
  } modelsel;

  struct {
    char msg[64];
    uint8_t r9mPower;
    BindInformation bindInformation;
    struct {
      union {
        uint8_t registerStep;
        uint8_t resetStep;
      };
      uint8_t registerPopupVerticalPosition;
      uint8_t registerPopupHorizontalPosition;
      int8_t registerPopupEditMode;
      char registerRxName[PXX2_LEN_RX_NAME];
      uint8_t registerLoopIndex; // will be removed later
      union {
        uint8_t shareReceiverIndex;
        uint8_t resetReceiverIndex;
      };
      uint8_t resetReceiverFlags;
      ModuleInformation moduleInformation;
    } pxx2;
#if defined(BLUETOOTH)
    struct {
      char devices[MAX_BLUETOOTH_DISTANT_ADDR][LEN_BLUETOOTH_ADDR+1];
      uint8_t devicesCount;
    } bt;
#endif
  } moduleSetup;

  struct {
    int16_t midVals[NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_MOUSE_ANALOGS];
    int16_t loVals[NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_MOUSE_ANALOGS];
    int16_t hiVals[NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_MOUSE_ANALOGS];
    uint8_t state;
#if defined(PCBTARANIS) || defined(PCBHORUS)
    struct {
      uint8_t stepsCount;
      int16_t steps[XPOTS_MULTIPOS_COUNT];
      uint8_t lastCount;
      int16_t lastPosition;
    } xpotsCalib[NUM_XPOTS];
#endif
  } calib;

#if defined(SDCARD)
  struct {
    char lines[NUM_BODY_LINES][SD_SCREEN_FILE_LENGTH+1+1]; // the last char is used to store the flags (directory) of the line
    uint32_t available;
    uint16_t offset;
    uint16_t count;
    char originalName[SD_SCREEN_FILE_LENGTH+1];
    OtaUpdateInformation otaUpdateInformation;
    char otaReceiverVersion[sizeof(TR_CURRENT_VERSION) + 12];
  } sdManager;
#endif

  struct {
    ModuleInformation modules[NUM_MODULES];
    uint32_t updateTime;

    union {
      ModuleSettings moduleSettings;
      ReceiverSettings receiverSettings;
    };

    uint8_t moduleSettingsDirty;

  } hardwareAndSettings;

  struct {
    ModuleInformation modules[NUM_MODULES];
    uint8_t linesCount;
  } radioTools;

  struct {
    uint8_t stickMode;
  } generalSettings;

  struct {
    uint8_t bars[LCD_W];
    uint32_t freq;
    uint32_t span;
    uint32_t step;
    uint8_t spanDefault;
    uint8_t spanMax;
    uint16_t freqDefault;
    uint16_t freqMax;
    uint16_t freqMin;
    uint8_t dirty;
  } spectrumAnalyser;

  struct {
    uint32_t freq;
    int16_t power;
    int16_t peak;
    uint8_t attn;
    uint8_t dirty;
  } powerMeter;

  struct {
    int8_t preset;
  } curveEdit;

#if defined(STM32)
  // Data for the USB mass storage driver. If USB mass storage runs no menu is not allowed to be displayed
  uint8_t MSC_BOT_Data[MSC_MEDIA_PACKET];
#endif
};

extern union ReusableBuffer reusableBuffer;

uint8_t zlen(const char *str, uint8_t size);
bool zexist(const char *str, uint8_t size);
unsigned int effectiveLen(const char * str, unsigned int size);
char * strcat_zchar(char *dest, const char *name, uint8_t size, const char *defaultName=NULL, uint8_t defaultNameSize=0, uint8_t defaultIdx=0);
#define strcatFlightmodeName(dest, idx) strcat_zchar(dest, g_model.flightModeData[idx].name, LEN_FLIGHT_MODE_NAME, STR_FM, PSIZE(TR_FM), idx+1)
#if defined(EEPROM)
#define strcat_modelname(dest, idx) strcat_zchar(dest, modelHeaders[idx].name, LEN_MODEL_NAME, STR_MODEL, PSIZE(TR_MODEL), idx+1)
#define strcat_currentmodelname(dest) strcat_modelname(dest, g_eeGeneral.currModel)
#else
#define strcat_currentmodelname(dest) strcat_zchar(dest, g_model.header.name, LEN_MODEL_NAME)
#endif
#define ZLEN(s) zlen(s, sizeof(s))
#define ZEXIST(s) zexist(s, sizeof(s))

// Stick tolerance varies between transmitters, Higher is better
#define STICK_TOLERANCE 64

#if defined(TELEMETRY_FRSKY)
  ls_telemetry_value_t minTelemValue(source_t channel);
  ls_telemetry_value_t maxTelemValue(source_t channel);
#else
  #define minTelemValue(channel) 255
  #define maxTelemValue(channel) 255
#endif

getvalue_t convert16bitsTelemValue(source_t channel, ls_telemetry_value_t value);
getvalue_t convertLswTelemValue(LogicalSwitchData * cs);

#define convertTelemValue(channel, value) convert16bitsTelemValue(channel, value)

inline int div_and_round(int num, int den)
{
  if (den == 0) {
    return 0;
  }
  else if (num >= 0) {
    num += den / 2;
  }
  else {
    num -= den / 2;
  }
  return num / den;
}

#if defined(TELEMETRY_FRSKY)
enum TelemetryViews {
  TELEMETRY_CUSTOM_SCREEN_1,
  TELEMETRY_CUSTOM_SCREEN_2,
  TELEMETRY_CUSTOM_SCREEN_3,
  TELEMETRY_CUSTOM_SCREEN_4,
  TELEMETRY_VIEW_MAX = TELEMETRY_CUSTOM_SCREEN_4
};

extern uint8_t s_frsky_view;
#endif

constexpr uint32_t EARTH_RADIUS = 6371009;

void getGpsPilotPosition();
void getGpsDistance();
void varioWakeup();

#if defined(AUDIO) && defined(BUZZER)
  #define IS_SOUND_OFF() (g_eeGeneral.buzzerMode==e_mode_quiet && g_eeGeneral.beepMode==e_mode_quiet)
#else
  #define IS_SOUND_OFF() (g_eeGeneral.beepMode == e_mode_quiet)
#endif

#define IS_IMPERIAL_ENABLE() (g_eeGeneral.imperial)

#if defined(PCBTARANIS)
  extern const unsigned char logo_taranis[];
#endif

#if defined(STM32)
void usbPluggedIn();
#endif

#include "lua/lua_api.h"

#if defined(SDCARD)
enum ClipboardType {
  CLIPBOARD_TYPE_NONE,
  CLIPBOARD_TYPE_CUSTOM_SWITCH,
  CLIPBOARD_TYPE_CUSTOM_FUNCTION,
  CLIPBOARD_TYPE_SD_FILE,
};

#if defined(SIMU)
  #define CLIPBOARD_PATH_LEN 1024
#else
  #define CLIPBOARD_PATH_LEN 32
#endif

struct Clipboard {
  ClipboardType type;
  union {
    LogicalSwitchData csw;
    CustomFunctionData cfn;
    struct {
      char directory[CLIPBOARD_PATH_LEN];
      char filename[CLIPBOARD_PATH_LEN];
    } sd;
  } data;
};

extern Clipboard clipboard;
#endif

#if !defined(SIMU)
extern uint16_t s_anaFilt[NUM_ANALOGS];
#endif

#if defined(JITTER_MEASURE)
extern JitterMeter<uint16_t> rawJitter[NUM_ANALOGS];
extern JitterMeter<uint16_t> avgJitter[NUM_ANALOGS];
#if defined(PCBHORUS)
  #define JITTER_MEASURE_ACTIVE()   (menuHandlers[menuLevel] == menuStatsAnalogs)
#elif defined(PCBTARANIS)
  #define JITTER_MEASURE_ACTIVE()   (menuHandlers[menuLevel] == menuRadioDiagAnalogs)
#elif defined(CLI)
  #define JITTER_MEASURE_ACTIVE()   (1)
#else
  #define JITTER_MEASURE_ACTIVE()   (0)
#endif
#endif

#if defined(INTERNAL_GPS)
  #include "gps.h"
#endif

#if defined(JACK_DETECT_GPIO)
enum JackMode {
  JACK_UNSELECTED_MODE,
  JACK_HEADPHONE_MODE,
  JACK_TRAINER_MODE,
  JACK_MAX_MODE = JACK_TRAINER_MODE
};
#endif

#if defined(GYRO)
#include "gyro.h"
#endif

inline bool isSimu()
{
#if defined(SIMU)
  return true;
#else
  return false;
#endif
}

#if defined(DEBUG_LATENCY)
extern uint8_t latencyToggleSwitch;
#endif

#endif // _OPENTX_H_
