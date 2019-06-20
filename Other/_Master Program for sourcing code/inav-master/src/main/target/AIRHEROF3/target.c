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

#include <stdbool.h>
#include <stdint.h>

#include <platform.h>
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"
#include "drivers/bus.h"

const timerHardware_t timerHardware[] = {
    DEF_TIM(TIM2,   CH1, PA0,  TIM_USE_PPM | TIM_USE_PWM, 0),
    DEF_TIM(TIM2,   CH2, PA1,  TIM_USE_PWM,               0),
    DEF_TIM(TIM2,   CH3, PA2,  TIM_USE_PWM,               0),
    DEF_TIM(TIM2,   CH4, PA3,  TIM_USE_PWM,               0),
    DEF_TIM(TIM3,   CH1, PA6,  TIM_USE_PWM | TIM_USE_ANY, 0),
    DEF_TIM(TIM3,   CH2, PA7,  TIM_USE_PWM,               0),

#if 1
    DEF_TIM(TIM1,   CH1, PA8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_MOTOR, 0), // DMA1_CH2
    DEF_TIM(TIM1,   CH4, PA11, TIM_USE_MC_MOTOR |                    TIM_USE_FW_MOTOR, 0), // DMA1_CH4
    DEF_TIM(TIM8,   CH1, PB6,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA2_CH3
    DEF_TIM(TIM4,   CH2, PB7,  TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0), // DMA1_CH4 - conflict with TIM1_CH4
#else
    DEF_TIM(TIM1,   CH1, PA8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_MOTOR, 0), // DMA1_CH2
    DEF_TIM(TIM4,   CH1, PA11, TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA1_CH1
    DEF_TIM(TIM8,   CH1, PB6,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA2_CH3
    DEF_TIM(TIM4,   CH2, PB7,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA1_CH4
#endif

#if !defined(AIRHEROF3_QUAD)
    DEF_TIM(TIM8,   CH2, PB8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA2_CH5
    DEF_TIM(TIM8,   CH3, PB9,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0), // DMA2_CH1
#endif
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
