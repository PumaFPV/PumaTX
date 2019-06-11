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

#include "platform.h"

#include "drivers/bus.h"
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"

const timerHardware_t timerHardware[] = {
    DEF_TIM(TIM5, CH4, PA3, TIM_USE_PPM,                         0, 0),                     // PPM

    DEF_TIM(TIM3, CH1, PC6, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR, 0, 0),                     // S1 D(1, 4, 5)
    DEF_TIM(TIM8, CH2, PC7, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0, 0),                     // S2 D(2, 3, 7)
    DEF_TIM(TIM8, CH3, PC8, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0, 0),                     // S3 D(2, 4, 7)
    DEF_TIM(TIM8, CH4, PC9, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0, 0),                     // S4 D(2, 7, 7)

    DEF_TIM(TIM3, CH4, PB1, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR, 0, 0),                     // S5 DMA1_ST2
#ifdef MATEKF722_HEXSERVO
    DEF_TIM(TIM1, CH1, PA8, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO, 0, 0),                     // S6 DMA2_ST6
#else
    DEF_TIM(TIM1, CH1, PA8, TIM_USE_MC_MOTOR | TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0, 0),  // S6 DMA2_ST6
#endif
    DEF_TIM(TIM4, CH3, PB8, TIM_USE_MC_SERVO | TIM_USE_FW_SERVO, 0, 0),                     // S7 DMA1_ST7

    // DEF_TIM(TIM5, CH3, PA2, TIM_USE_ANY, 0, 0),                                           // TX2/S8  DMA1_ST0

    DEF_TIM(TIM2, CH1, PA15, TIM_USE_LED, 0, 0),                                            // LED STRIP  D(1, 5, 3)
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
