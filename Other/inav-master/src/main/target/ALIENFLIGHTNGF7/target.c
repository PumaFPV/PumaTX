/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include <platform.h>
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"

const timerHardware_t timerHardware[] = {
    DEF_TIM(TIM1, CH1,  PA8,  TIM_USE_PPM | TIM_USE_LED,            0, 1), // PPM   - DMA2_ST1
    DEF_TIM(TIM8, CH1,  PC6,  TIM_USE_MC_SERVO,                     0, 0), // PWM1  - DMA2_ST2
    DEF_TIM(TIM3, CH2,  PC7,  TIM_USE_MC_SERVO,                     0, 0), // PWM2  - DMA1_ST5
    DEF_TIM(TIM8, CH2N, PB14, TIM_USE_MC_SERVO,                     0, 0), // PWM3  - DMA2_ST3
    DEF_TIM(TIM3, CH3,  PB0,  TIM_USE_MC_SERVO,                     0, 0), // PWM4  - DMA1_ST7
    DEF_TIM(TIM5, CH1,  PA0,  TIM_USE_MC_SERVO,                     0, 0), // PWM5  - DMA1_ST2
    DEF_TIM(TIM8, CH3,  PC8,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM6  - DMA2_ST4
    DEF_TIM(TIM5, CH2,  PA1,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM7  - DMA1_ST4
    DEF_TIM(TIM8, CH4,  PC9,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM8  - DMA2_ST7
    DEF_TIM(TIM3, CH4,  PB1,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM9  - DMA1_ST2
    DEF_TIM(TIM1, CH3N, PB15, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM10 - DMA2_ST6
    DEF_TIM(TIM4, CH3,  PB8,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM11 - DMA1_ST7
    DEF_TIM(TIM4, CH4,  PB9,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,  0, 0), // PWM12 - DMA_NONE
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
