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

#ifndef _TASKS_ARM_H_
#define _TASKS_ARM_H_

#include "rtos.h"

// stack sizes should be in multiples of 8 for better alignment
#define MENUS_STACK_SIZE       2000
#define MIXER_STACK_SIZE       512
#define AUDIO_STACK_SIZE       512
#define CLI_STACK_SIZE         1000  // only consumed with CLI build option

#define MIXER_TASK_PRIO        5
#define AUDIO_TASK_PRIO        7
#define MENUS_TASK_PRIO        10
#define CLI_TASK_PRIO          10

extern RTOS_TASK_HANDLE menusTaskId;
extern RTOS_DEFINE_STACK(menusStack, MENUS_STACK_SIZE);

extern RTOS_TASK_HANDLE mixerTaskId;
extern RTOS_DEFINE_STACK(mixerStack, MIXER_STACK_SIZE);

extern RTOS_TASK_HANDLE audioTaskId;
extern RTOS_DEFINE_STACK(audioStack, AUDIO_STACK_SIZE);

extern RTOS_MUTEX_HANDLE mixerMutex;
extern RTOS_FLAG_HANDLE openTxInitCompleteFlag;

void stackPaint();
void tasksStart();

extern volatile uint16_t timeForcePowerOffPressed;
inline void resetForcePowerOffRequest()
{
  timeForcePowerOffPressed = 0;
}

#endif // _TASKS_ARM_H_
