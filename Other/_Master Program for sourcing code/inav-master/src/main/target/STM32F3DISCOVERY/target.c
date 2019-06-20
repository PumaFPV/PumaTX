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
    DEF_TIM(TIM1,  CH1, PA8,  TIM_USE_PWM | TIM_USE_PPM,                                0),
    DEF_TIM(TIM16, CH1, PB8,  TIM_USE_PWM | TIM_USE_LED,                                0),
    DEF_TIM(TIM17, CH1, PB9,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM8,  CH1, PC6,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM8,  CH2, PC7,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM8,  CH3, PC8,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM3,  CH4, PB1,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM3,  CH2, PA4,  TIM_USE_PWM,                                              0),
    DEF_TIM(TIM4,  CH1, PD12, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR,                      0),
    DEF_TIM(TIM4,  CH2, PD13, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR,                      0),
    DEF_TIM(TIM4,  CH3, PD14, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0),
    DEF_TIM(TIM4,  CH4, PD15, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO,                      0),
    DEF_TIM(TIM2,  CH2, PA1,  TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO,   0),
    DEF_TIM(TIM2,  CH3, PA2,  TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO,   0),
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
