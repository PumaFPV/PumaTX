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

#define SIMPGMSPC_USE_QT    0

#include "opentx.h"
#include <errno.h>
#include <stdarg.h>
#include <string>

#if !defined (_MSC_VER) || defined (__GNUC__)
  #include <chrono>
  #include <sys/time.h>
#endif

#if defined(SIMU_AUDIO)
  #include <SDL.h>
#endif

uint8_t MCUCSR, MCUSR, MCUCR;
volatile uint8_t pina=0xff, pinb=0xff, pinc=0xff, pind, pine=0xff, pinf=0xff, ping=0xff, pinh=0xff, pinj=0, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
int g_snapshot_idx = 0;

uint8_t simu_start_mode = 0;
char * main_thread_error = NULL;

bool simu_shutdown = false;
bool simu_running = false;

#if defined(STM32)
uint32_t Peri1_frequency, Peri2_frequency;
GPIO_TypeDef gpioa, gpiob, gpioc, gpiod, gpioe, gpiof, gpiog, gpioh, gpioi, gpioj;
TIM_TypeDef tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8, tim9, tim10;
RCC_TypeDef rcc;
DMA_Stream_TypeDef dma1_stream1, dma1_stream2, dma1_stream3, dma1_stream4, dma1_stream5, dma1_stream6, dma1_stream7, dma2_stream1, dma2_stream2, dma2_stream5, dma2_stream6, dma2_stream7;
DMA_TypeDef dma2;
USART_TypeDef Usart0, Usart1, Usart2, Usart3, Usart4;
SysTick_Type systick;
#else
Pio Pioa, Piob, Pioc;
Pmc pmc;
Ssc ssc;
Pwm pwm;
Tc tc1;
Twi Twio;
Usart Usart0;
Dacc dacc;
Adc Adc0;
#endif

FATFS g_FATFS_Obj;

void lcdInit()
{
}

void toplcdOff()
{
}

uint64_t simuTimerMicros(void)
{
#if SIMPGMSPC_USE_QT

  static QElapsedTimer ticker;
  if (!ticker.isValid())
    ticker.start();
  return ticker.nsecsElapsed() / 1000;

#elif defined(_MSC_VER)

  static double freqScale = 0.0;
  static LARGE_INTEGER firstTick;
  LARGE_INTEGER newTick;

  if (!freqScale) {
    LARGE_INTEGER frequency;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    // 1us resolution
    freqScale = 1e6 / frequency.QuadPart;
    // init timer
    QueryPerformanceCounter(&firstTick);
    TRACE_SIMPGMSPACE("microsTimer() init: first tick = %llu @ %llu Hz", firstTick.QuadPart, frequency.QuadPart);
  }
  // read the timer
  QueryPerformanceCounter(&newTick);
  // compute the elapsed time
  return (newTick.QuadPart - firstTick.QuadPart) * freqScale;

#else  // GNUC

  auto now = std::chrono::steady_clock::now();
  return (uint64_t) std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

#endif
}

uint16_t getTmr16KHz()
{
  return simuTimerMicros() * 2 / 125;
}

uint16_t getTmr2MHz()
{
  return simuTimerMicros() * 2;
}

// return 2ms resolution to match CoOS settings
uint64_t CoGetOSTime(void)
{
  return simuTimerMicros() / 2000;
}

void simuInit()
{
#if defined(STM32)
  RCC->CSR = 0;
#endif

  // set power button to "not pressed"
#if defined(PWR_SWITCH_GPIO)  // STM32
  GPIO_SetBits(PWR_SWITCH_GPIO, PWR_SWITCH_GPIO_PIN);
#elif defined(PIO_PC17)       // AT91SAM3
  PIOC->PIO_PDSR &= ~PIO_PC17;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  rotencValue = 0;
#endif
}

bool keysStates[NUM_KEYS] = { false };
void simuSetKey(uint8_t key, bool state)
{
  keysStates[key] = state;
}

bool trimsStates[NUM_TRIMS*2] = { false };
void simuSetTrim(uint8_t trim, bool state)
{
  trimsStates[trim] = state;
}

int8_t switchesStates[NUM_SWITCHES] = { -1 };
void simuSetSwitch(uint8_t swtch, int8_t state)
{
  // TRACE("simuSetSwitch(%d, %d)", swtch, state);
  switchesStates[swtch] = state;
}

void StartSimu(bool tests, const char * sdPath, const char * settingsPath)
{
  if (simu_running)
    return;

  stopPulses();
  menuLevel = 0;

  simu_start_mode = (tests ? 0 : OPENTX_START_NO_SPLASH | OPENTX_START_NO_CALIBRATION | OPENTX_START_NO_CHECKS);
  simu_shutdown = false;

  simuFatfsSetPaths(sdPath, settingsPath);

  /*
    g_tmr10ms must be non-zero otherwise some SF functions (that use this timer as a marker when it was last executed)
    will be executed twice on startup. Normal radio does not see this issue because g_tmr10ms is already a big number
    before the first call to the Special Functions. Not so in the simulator.

    There is another issue, some other function static variables depend on this value. If simulator is started
    multiple times in one Companion session, they are set to their initial values only first time the simulator
    is started. Therefore g_tmr10ms must also be set to non-zero value only the first time, then it must be left
    alone to continue from the previous simulator session value. See the issue #2446

  */
  if (g_tmr10ms == 0) {
    g_tmr10ms = 1;
  }

#if defined(RTCLOCK)
  g_rtcTime = time(0);
#endif

#if defined(SIMU_EXCEPTIONS)
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);
  try {
#endif

  simuMain();

  simu_running = true;

#if defined(SIMU_EXCEPTIONS)
  }
  catch (...) {
  }
#endif
}

void StopSimu()
{
  if (!simu_running)
    return;

  simu_shutdown = true;

  pthread_join(mixerTaskId, NULL);
  pthread_join(menusTaskId, NULL);

  simu_running = false;
}

struct SimulatorAudio {
  int volumeGain;
  int currentVolume;
  uint16_t leftoverData[AUDIO_BUFFER_SIZE];
  int leftoverLen;
  bool threadRunning;
  pthread_t threadPid;
} simuAudio;

bool simuIsRunning()
{
  return simu_running;
}

uint8_t simuSleep(uint32_t ms)
{
  for (uint32_t i = 0; i < ms; ++i){
    if (simu_shutdown || !simu_running)
      return 1;
    sleep(1);
  }
  return 0;
}

void audioConsumeCurrentBuffer()
{
}

void setScaledVolume(uint8_t volume)
{
  simuAudio.currentVolume = 127 * volume * simuAudio.volumeGain / VOLUME_LEVEL_MAX / 10;
  // TRACE_SIMPGMSPACE("setVolume(): in: %u, out: %u", volume, simuAudio.currentVolume);
}

void setVolume(uint8_t volume)
{
}

int32_t getVolume()
{
  return 0;
}

#if defined(SIMU_AUDIO)
void copyBuffer(uint8_t * dest, const uint16_t * buff, unsigned int samples)
{
  for(unsigned int i=0; i<samples; i++) {
    int sample = ((int32_t)(uint32_t)(buff[i]) - 0x8000);  // conversion from uint16_t
    *((uint16_t*)dest) = (int16_t)((sample * simuAudio.currentVolume)/127);
    dest += 2;
  }
}

void fillAudioBuffer(void *udata, Uint8 *stream, int len)
{
  SDL_memset(stream, 0, len);

  if (simuAudio.leftoverLen) {
    int len1 = min(len/2, simuAudio.leftoverLen);
    copyBuffer(stream, simuAudio.leftoverData, len1);
    len -= len1*2;
    stream += len1*2;
    simuAudio.leftoverLen -= len1;
    // putchar('l');
    if (simuAudio.leftoverLen) return;		// buffer fully filled
  }

  if (audioQueue.buffersFifo.filledAtleast(len/(AUDIO_BUFFER_SIZE*2)+1) ) {
    while(true) {
      const AudioBuffer * nextBuffer = audioQueue.buffersFifo.getNextFilledBuffer();
      if (nextBuffer) {
        if (len >= nextBuffer->size*2) {
          copyBuffer(stream, nextBuffer->data, nextBuffer->size);
          stream += nextBuffer->size*2;
          len -= nextBuffer->size*2;
          // putchar('+');
          audioQueue.buffersFifo.freeNextFilledBuffer();
        }
        else {
          //partial
          copyBuffer(stream, nextBuffer->data, len/2);
          simuAudio.leftoverLen = (nextBuffer->size-len/2);
          memcpy(simuAudio.leftoverData, &nextBuffer->data[len/2], simuAudio.leftoverLen*2);
          len = 0;
          // putchar('p');
          audioQueue.buffersFifo.freeNextFilledBuffer();
          break;
        }
      }
      else {
        break;
      }
    }
  }

  //fill the rest of buffer with silence
  if (len > 0) {
    SDL_memset(stream, 0x8000, len);  // make sure this is silence.
    // putchar('.');
  }
}

void * audioThread(void *)
{
  /*
    Checking here if SDL audio was initialized is wrong, because
    the SDL_CloseAudio() de-initializes it.

    if ( !SDL_WasInit(SDL_INIT_AUDIO) ) {
      fprintf(stderr, "ERROR: couldn't initialize SDL audio support\n");
      return 0;
    }
  */

  SDL_AudioSpec wanted, have;

  /* Set the audio format */
  wanted.freq = AUDIO_SAMPLE_RATE;
  wanted.format = AUDIO_S16SYS;
  wanted.channels = 1;    /* 1 = mono, 2 = stereo */
  wanted.samples = AUDIO_BUFFER_SIZE*2;  /* Good low-latency value for callback */
  wanted.callback = fillAudioBuffer;
  wanted.userdata = NULL;

  /*
    SDL_OpenAudio() internally calls SDL_InitSubSystem(SDL_INIT_AUDIO),
    which initializes SDL Audio subsystem if necessary
  */
  if ( SDL_OpenAudio(&wanted, &have) < 0 ) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    return 0;
  }
  SDL_PauseAudio(0);

  while (simuAudio.threadRunning) {
    audioQueue.wakeup();
    sleep(1);
  }
  SDL_CloseAudio();
  return 0;
}

void StartAudioThread(int volumeGain)
{
  simuAudio.leftoverLen = 0;
  simuAudio.threadRunning = true;
  simuAudio.volumeGain = volumeGain;
  TRACE_SIMPGMSPACE("StartAudioThread(%d)", volumeGain);
  setScaledVolume(VOLUME_LEVEL_DEF);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  struct sched_param sp;
  sp.sched_priority = SCHED_RR;
  pthread_attr_setschedparam(&attr, &sp);
  pthread_create(&simuAudio.threadPid, &attr, &audioThread, nullptr);
  pthread_setname_np(simuAudio.threadPid, "audio");
  return;
}

void StopAudioThread()
{
  simuAudio.threadRunning = false;
  pthread_join(simuAudio.threadPid, NULL);
}
#endif // #if defined(SIMU_AUDIO)

bool simuLcdRefresh = true;
display_t simuLcdBuf[DISPLAY_BUFFER_SIZE];

#if !defined(COLORLCD)
void lcdSetRefVolt(uint8_t val)
{
}
#endif

void adcPrepareBandgap()
{
}

#if defined(PCBTARANIS)
void lcdOff()
{
}
#endif

void lcdRefresh()
{
  static bool lightEnabled = (bool)isBacklightEnabled();

  if (bool(isBacklightEnabled()) != lightEnabled || memcmp(simuLcdBuf, displayBuf, DISPLAY_BUFFER_SIZE * sizeof(display_t))) {
    memcpy(simuLcdBuf, displayBuf, DISPLAY_BUFFER_SIZE * sizeof(display_t));
    lightEnabled = (bool)isBacklightEnabled();
    simuLcdRefresh = true;
  }
}

void telemetryPortInit(uint8_t baudrate)
{
}

void telemetryPortInit()
{
}

display_t simuLcdBackupBuf[DISPLAY_BUFFER_SIZE];
void lcdStoreBackupBuffer()
{
  memcpy(simuLcdBackupBuf, displayBuf, sizeof(simuLcdBackupBuf));
}

int lcdRestoreBackupBuffer()
{
  memcpy(displayBuf, simuLcdBackupBuf, sizeof(displayBuf));
  return 1;
}

uint32_t pwrCheck()
{
  // TODO: ability to simulate shutdown warning for a "soft" simulator restart
  return simu_shutdown ? e_power_off : e_power_on;
}

void pwrOff()
{
}

uint32_t pwrPressed()
{
  // TODO: simulate power button
#if defined(PWR_SWITCH_GPIO)  // STM32
  return GPIO_ReadInputDataBit(PWR_SWITCH_GPIO, PWR_SWITCH_GPIO_PIN) == Bit_RESET;
#elif defined(PIO_PC17)       // AT91SAM3
  return PIOC->PIO_PDSR & PIO_PC17;
#else
  return false;
#endif
}

void pwrInit()
{
}

void readKeysAndTrims()
{
  uint8_t index = 0;
  uint32_t in = readKeys();
  for (uint8_t i = 1; i != uint8_t(1 << TRM_BASE); i <<= 1) {
    keys[index++].input(in & i);
  }

  in = readTrims();
  for (uint8_t i = 1; i != uint8_t(1 << 8); i <<= 1) {
    keys[index++].input(in & i);
  }
}

bool keyDown()
{
  return readKeys();
}

bool trimDown(uint8_t idx)
{
  return readTrims() & (1 << idx);
}

uint32_t readKeys()
{
  uint32_t result = 0;

  for (int i=0; i<NUM_KEYS; i++) {
    if (keysStates[i]) {
      // TRACE("key pressed %d", i);
      result |= 1 << i;
    }
  }

  return result;
}

uint32_t readTrims()
{
  uint32_t result = 0;

  for (int i=0; i<NUM_TRIMS*2; i++) {
    if (trimsStates[i]) {
      // TRACE("trim pressed %d", i);
      result |= 1 << i;
    }
  }

  return result;
}

uint32_t switchState(uint8_t index)
{
#if defined(PCBSKY9X)
  switch(index) {
    case 0:
      return switchesStates[0] < 0;
    case 1:
      return switchesStates[0] == 0;
    case 2:
      return switchesStates[0] > 0;
    default:
      return switchesStates[index - 2] > 0;
  }
#else
  div_t qr = div(index, 3);
  int state = switchesStates[qr.quot];
  switch (qr.rem) {
    case 0:
      return state < 0;
    case 2:
      return state > 0;
    default:
      return state == 0;
  }
#endif
}

#if defined(STM32)
int usbPlugged() { return false; }
int getSelectedUsbMode() { return USB_JOYSTICK_MODE; }
void setSelectedUsbMode(int mode) {}
void delay_ms(uint32_t ms) { }

// GPIO fake functions
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF) { }

// PWR fake functions
void PWR_BackupAccessCmd(FunctionalState NewState) { }
void PWR_BackupRegulatorCmd(FunctionalState NewState) { }

// USART fake functions
void USART_DeInit(USART_TypeDef* ) { }
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct) { }
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState) { }
void USART_ClearITPendingBit(USART_TypeDef*, unsigned short) { }
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data) { }
uint16_t USART_ReceiveData(USART_TypeDef*) { return 0; }
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState) { }
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState) { }
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG) { return SET; }

// TIM fake functions
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength) { }
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState) { }
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState) { }

// I2C fake functions
void I2C_DeInit(I2C_TypeDef*) { }
void I2C_Init(I2C_TypeDef*, I2C_InitTypeDef*) { }
void I2C_Cmd(I2C_TypeDef*, FunctionalState) { }
void I2C_Send7bitAddress(I2C_TypeDef*, unsigned char, unsigned char) { }
void I2C_SendData(I2C_TypeDef*, unsigned char) { }
void I2C_GenerateSTART(I2C_TypeDef*, FunctionalState) { }
void I2C_GenerateSTOP(I2C_TypeDef*, FunctionalState) { }
void I2C_AcknowledgeConfig(I2C_TypeDef*, FunctionalState) { }
uint8_t I2C_ReceiveData(I2C_TypeDef*) { return 0; }
ErrorStatus I2C_CheckEvent(I2C_TypeDef*, unsigned int) { return (ErrorStatus) ERROR; }

// I2S fake functions
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct) { }
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState) { }

// SPI fake functions
void SPI_I2S_DeInit(SPI_TypeDef* SPIx) { }
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState) { }

// RCC fake functions
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource) { }
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState) { }
void RCC_RTCCLKCmd(FunctionalState NewState) { }
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR) { }
void RCC_PLLI2SCmd(FunctionalState NewState) { }
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource) { }
void RCC_LSEConfig(uint8_t RCC_LSE) { }
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks) { };
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG) { return SET; }

// EXTI fake functions
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex) { }
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct) { }
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line) { return RESET; }
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct) { }
void EXTI_ClearITPendingBit(uint32_t EXTI_Line) { }

// RTC fake functions
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) { return SUCCESS; }
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct) { }
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct) { }
ErrorStatus RTC_WaitForSynchro(void) { return SUCCESS; }
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct) { return SUCCESS; }
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct) { return SUCCESS; }
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef * RTC_TimeStruct)
{
  time_t tme;
  time(&tme);
  struct tm * timeinfo = localtime(&tme);
  RTC_TimeStruct->RTC_Hours = timeinfo->tm_hour;
  RTC_TimeStruct->RTC_Minutes = timeinfo->tm_min;
  RTC_TimeStruct->RTC_Seconds = timeinfo->tm_sec;
}

void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef * RTC_DateStruct)
{
  time_t tme;
  time(&tme);
  struct tm * timeinfo = localtime(&tme);
  RTC_DateStruct->RTC_Year = timeinfo->tm_year - 100; // STM32 year is two decimals only (so base is currently 2000), tm is based on number of years since 1900
  RTC_DateStruct->RTC_Month = timeinfo->tm_mon + 1;
  RTC_DateStruct->RTC_Date = timeinfo->tm_mday;
}


void unlockFlash() { }
void lockFlash() { }
void flashWrite(uint32_t *address, uint32_t *buffer) { simuSleep(100); }
uint32_t isBootloaderStart(const uint8_t * block) { return 1; }
#endif // defined(STM32)

#if defined(PCBHORUS)
void LCD_ControlLight(uint16_t dutyCycle) { }
#endif

#if defined(PCBXLITES)
bool isJackPlugged() { return false; }
#endif

void serialPrintf(const char * format, ...) { }
void serialCrlf() { }
void serialPutc(char c) { }
