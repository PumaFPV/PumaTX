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
    DEF_TIM(TIM3,  CH2,  PA4, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR, 0),                      // PWM1  - PA4  - *TIM3_CH2
    DEF_TIM(TIM3,  CH1,  PA6, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR, 0),                      // PWM2  - PA6  - *TIM3_CH1, TIM8_BKIN, TIM1_BKIN, TIM16_CH1
    DEF_TIM(TIM8,  CH2N, PB0, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0),                      // PWM3  - PB0  - *TIM3_CH3, TIM1_CH2N, TIM8_CH2N
    DEF_TIM(TIM8,  CH3N, PB1, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0),                      // PWM4  - PB1  - *TIM3_CH4, TIM1_CH3N, TIM8_CH3N
    DEF_TIM(TIM2,  CH2,  PA1, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0),                      // PWM5  - PA1  - *TIM2_CH2, TIM15_CH1N
    DEF_TIM(TIM2,  CH3,  PA2, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0),                      // PWM6  - PA2  - *TIM2_CH3, !TIM15_CH1
    DEF_TIM(TIM15, CH2,  PA3, TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0),   // PWM7  - PA3  - *TIM15_CH2, TIM2_CH4
    DEF_TIM(TIM1,  CH1,  PA8, TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0),   // PWM8  - PA8  - *TIM1_CH1, TIM4_ETR
    DEF_TIM(TIM17, CH1,  PA7, TIM_USE_PPM,   0),                                            // PPM   - PA7  - *TIM17_CH1, TIM1_CH1N, TIM8_CH1
    DEF_TIM(TIM16, CH1,  PB8, TIM_USE_LED,   0),                                            // LED   - PB8  - *TIM16_CH1, TIM4_CH3, TIM8_CH2
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
