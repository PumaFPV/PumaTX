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
#include <platform.h>
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"

const timerHardware_t timerHardware[] = {
    DEF_TIM(TIM9, CH2, PA3,  TIM_USE_PPM,   0, 0), // PPM

#ifdef MATEKF405_SERVOS6
    DEF_TIM(TIM3, CH1, PC6,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_MOTOR, 0, 0), // S1
#else
    DEF_TIM(TIM3, CH1, PC6,  TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_MOTOR, 0, 0), // S1
#endif
    DEF_TIM(TIM8, CH2, PC7,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0, 1), // S2  UP(2,1)
    DEF_TIM(TIM8, CH3, PC8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0, 1), // S3  UP(2,1)
    DEF_TIM(TIM8, CH4, PC9,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0, 0), // S4  UP(2,1)

    DEF_TIM(TIM2, CH1, PA15, TIM_USE_MC_MOTOR | TIM_USE_LED                        , 0, 0), // S5  UP(1,7)

#ifdef MATEKF405_SERVOS6
    DEF_TIM(TIM1, CH1, PA8,  TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0, 0), // S6  UP(2,5)
#else
    DEF_TIM(TIM1, CH1, PA8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_SERVO, 0, 0), // S6  UP(2,5)
#endif
    DEF_TIM(TIM4, CH3, PB8,  TIM_USE_MC_MOTOR |                    TIM_USE_FW_MOTOR, 0, 0), // S7  D(1,7)!S5 UP(2,6)

    DEF_TIM(TIM5, CH3, PA2,  TIM_USE_PWM,                                            0, 0), // TX2

    DEF_TIM(TIM5, CH1, PA0,  TIM_USE_PWM,                                            0, 0),   // TX4 UP(1,6) D(1,2)!S1
    DEF_TIM(TIM5, CH2, PA1,  TIM_USE_PWM,                                            0, 0),   // RX4 UP(1,6) D(1,4)
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
