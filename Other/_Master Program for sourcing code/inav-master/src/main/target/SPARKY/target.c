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
    // 6 3-pin headers
    DEF_TIM(TIM15, CH2, PB15, TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_MOTOR,   0), // PWM1  - PB15 - TIM1_CH3N, TIM15_CH1N, *TIM15_CH2
    DEF_TIM(TIM15, CH1, PB14, TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_MOTOR,   0), // PWM2  - PB14 - TIM1_CH2N, *TIM15_CH1
    DEF_TIM(TIM1,  CH1, PA8,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM3  - PA8  - *TIM1_CH1, TIM4_ETR
    DEF_TIM(TIM3,  CH3, PB0,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM4  - PB0  - *TIM3_CH3, TIM1_CH2N, TIM8_CH2N
    DEF_TIM(TIM16, CH1, PA6,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO | TIM_USE_LED,        0), // PWM5  - PA6  - *TIM3_CH1, TIM8_BKIN, TIM1_BKIN, TIM16_CH1
    DEF_TIM(TIM2,  CH3, PA2,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM6  - PA2  - *TIM2_CH3, !TIM15_CH1

    // PWM7 - PMW10
    DEF_TIM(TIM3,  CH4, PB1,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM7  - PB1  - *TIM3_CH4, TIM1_CH3N, TIM8_CH3N
    DEF_TIM(TIM17, CH1, PA7,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM8  - PA7  - !TIM3_CH2, *TIM17_CH1, TIM1_CH1N, TIM8_CH1
    DEF_TIM(TIM3,  CH2, PA4,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM9  - PA4  - *TIM3_CH2
    DEF_TIM(TIM2,  CH2, PA1,  TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0), // PWM10 - PA1  - *TIM2_CH2, TIM15_CH1N

    // PPM PORT - Also USART2 RX (AF5)
    DEF_TIM(TIM2,  CH4, PA3,  TIM_USE_PPM,                                              0), // PPM   - PA3  - TIM2_CH4, TIM15_CH2 - PWM13
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
