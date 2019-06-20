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

// Start TIMER at 2000000Hz
void init2MhzTimer()
{
  TIMER_2MHz_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1; // 0.5 uS, 2 MHz
  TIMER_2MHz_TIMER->ARR = 65535;
  TIMER_2MHz_TIMER->CR2 = 0;
  TIMER_2MHz_TIMER->CR1 = TIM_CR1_CEN;
}

// Start TIMER at 200Hz
void init5msTimer()
{
  INTERRUPT_xMS_TIMER->ARR = 4999; // 5mS in uS
  INTERRUPT_xMS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1;  // 1uS
  INTERRUPT_xMS_TIMER->CCER = 0;
  INTERRUPT_xMS_TIMER->CCMR1 = 0;
  INTERRUPT_xMS_TIMER->EGR = 0;
  INTERRUPT_xMS_TIMER->CR1 = TIM_CR1_CEN | TIM_CR1_URS;
  INTERRUPT_xMS_TIMER->DIER |= TIM_DIER_UIE;
  NVIC_EnableIRQ(INTERRUPT_xMS_IRQn);
  NVIC_SetPriority(INTERRUPT_xMS_IRQn, 7);
}

void stop5msTimer()
{
  INTERRUPT_xMS_TIMER->CR1 = 0 ;        // stop timer
  NVIC_DisableIRQ(INTERRUPT_xMS_IRQn) ;
}

// TODO use the same than board_sky9x.cpp
void interrupt5ms()
{
  static uint8_t pre_scale;       // Used to get 10 Hz counter

  ++pre_scale;

#if defined(HAPTIC)
  DEBUG_TIMER_START(debugTimerHaptic);
  HAPTIC_HEARTBEAT();
  DEBUG_TIMER_STOP(debugTimerHaptic);
#endif

  if (pre_scale == 2) {
    pre_scale = 0;
    DEBUG_TIMER_START(debugTimerPer10ms);
    DEBUG_TIMER_SAMPLE(debugTimerPer10msPeriod);
    per10ms();
    DEBUG_TIMER_STOP(debugTimerPer10ms);
  }
}

extern "C" void INTERRUPT_xMS_IRQHandler()
{
  INTERRUPT_xMS_TIMER->SR &= ~TIM_SR_UIF;
  interrupt5ms();
  DEBUG_INTERRUPT(INT_1MS);
}
