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

#ifndef _RTOS_H_
#define _RTOS_H_

#include "definitions.h"

#ifdef __cplusplus
extern "C++" {
#endif

#if defined(SIMU)

  #include <pthread.h>
  #include <semaphore.h>

  #define SIMU_SLEEP_OR_EXIT_MS(x)       simuSleep(x)
  #define RTOS_MS_PER_TICK  1

  typedef pthread_t RTOS_TASK_HANDLE;
  typedef pthread_mutex_t RTOS_MUTEX_HANDLE;
  typedef uint32_t RTOS_FLAG_HANDLE;
  typedef sem_t * RTOS_EVENT_HANDLE;

  extern uint64_t simuTimerMicros(void);
  extern uint8_t simuSleep(uint32_t ms);

  static inline void RTOS_INIT()
  {
  }

  static inline void RTOS_START()
  {
  }

  static inline void RTOS_WAIT_MS(uint32_t x)
  {
    simuSleep(x);
  }

  static inline void RTOS_WAIT_TICKS(uint32_t x)
  {
    RTOS_WAIT_MS(x * RTOS_MS_PER_TICK);
  }

#ifdef __cplusplus
  static inline void RTOS_CREATE_MUTEX(pthread_mutex_t &mutex)
  {
    mutex = PTHREAD_MUTEX_INITIALIZER;
  }

  static inline void RTOS_LOCK_MUTEX(pthread_mutex_t &mutex)
  {
      pthread_mutex_lock(&mutex);
  }

  static inline void RTOS_UNLOCK_MUTEX(pthread_mutex_t &mutex)
  {
      pthread_mutex_unlock(&mutex);
  }

  static inline void RTOS_CREATE_FLAG(uint32_t &flag)
  {
    flag = 0; // TODO: real flags (use semaphores?)
  }

  static inline void RTOS_SET_FLAG(uint32_t &flag)
  {
    flag = 1;
  }

  template<int SIZE>
  class FakeTaskStack
  {
    public:
      FakeTaskStack()
      {
      }

      void paint()
      {
      }

      uint32_t size()
      {
        return SIZE;
      }

      uint32_t available()
      {
        return SIZE / 2;
      }
  };
  #define RTOS_DEFINE_STACK(name, size) FakeTaskStack<size> name

  #define TASK_FUNCTION(task)           void * task(void * pdata)

  template<int SIZE>
  inline void RTOS_CREATE_TASK(pthread_t &taskId, void * task(void *), const char * name, FakeTaskStack<SIZE> &, unsigned, unsigned)
  {
    pthread_create(&taskId, nullptr, task, nullptr);
    pthread_setname_np(taskId, name);
  }

  #define TASK_RETURN()                 return nullptr

  constexpr uint32_t stackAvailable()
  {
    return 500;
  }
#endif  // __cplusplus

  // return 2ms resolution to match CoOS settings
  static inline uint32_t RTOS_GET_TIME(void)
  {
    return (uint32_t)(simuTimerMicros() / 2000);
  }

  static inline uint32_t RTOS_GET_MS(void)
  {
    return (uint32_t)(simuTimerMicros() / 1000);
  }

#elif defined(RTOS_COOS)

#ifdef __cplusplus
  extern "C" {
#endif
    #include <CoOS.h>
#ifdef __cplusplus
  }
#endif

  #define RTOS_MS_PER_TICK              ((CFG_CPU_FREQ / CFG_SYSTICK_FREQ) / (CFG_CPU_FREQ / 1000))  // RTOS timer tick length in ms (currently 2)

  typedef OS_TID RTOS_TASK_HANDLE;
  typedef OS_MutexID RTOS_MUTEX_HANDLE;
  typedef OS_FlagID RTOS_FLAG_HANDLE;
  typedef OS_EventID RTOS_EVENT_HANDLE;

  static inline void RTOS_INIT()
  {
    CoInitOS();
  }

  static inline void RTOS_START()
  {
    CoStartOS();
  }

  static inline void RTOS_WAIT_MS(uint32_t x)
  {
    if (!x)
      return;
    if ((x = x / RTOS_MS_PER_TICK) < 1)
      x = 1;
    CoTickDelay(x);
  }

  static inline void RTOS_WAIT_TICKS(uint32_t x)
  {
    CoTickDelay(x);
  }

  #define RTOS_CREATE_TASK(taskId, task, name, stackStruct, stackSize, priority)   \
                                        taskId = CoCreateTask(task, NULL, priority, &stackStruct.stack[stackSize-1], stackSize)

#ifdef __cplusplus
  static inline void RTOS_CREATE_MUTEX(OS_MutexID &mutex)
  {
    mutex = CoCreateMutex();
  }

  static inline void RTOS_LOCK_MUTEX(OS_MutexID &mutex)
  {
    CoEnterMutexSection(mutex);
  }

  static inline void RTOS_UNLOCK_MUTEX(OS_MutexID &mutex)
  {
    CoLeaveMutexSection(mutex);
  }
#endif  // __cplusplus

  static inline uint32_t getStackAvailable(void * address, uint32_t size)
  {
    uint32_t * array = (uint32_t *)address;
    uint32_t i = 0;
    while (i < size && array[i] == 0x55555555) {
      i++;
    }
    return i;
  }

  extern int _estack;
  extern int _main_stack_start;
  static inline uint32_t stackSize()
  {
    return ((unsigned char *)&_estack - (unsigned char *)&_main_stack_start) / 4;
  }

  static inline uint32_t stackAvailable()
  {
    return getStackAvailable(&_main_stack_start, stackSize());
  }

  #define RTOS_CREATE_FLAG(flag)        flag = CoCreateFlag(false, false)
  #define RTOS_SET_FLAG(flag)           (void)CoSetFlag(flag)

#ifdef __cplusplus
  template<int SIZE>
  class TaskStack
  {
    public:
      TaskStack()
      {
      }

      void paint()
      {
        for (uint32_t i=0; i<SIZE; i++) {
          stack[i] = 0x55555555;
        }
      }

      uint32_t size()
      {
        return SIZE * 4;
      }

      uint32_t available()
      {
        return getStackAvailable(stack, SIZE);
      }

      OS_STK stack[SIZE];
  };
#endif // __cplusplus

  static inline uint32_t RTOS_GET_TIME(void)
  {
    return (uint32_t)CoGetOSTime();
  }

  static inline uint32_t RTOS_GET_MS(void)
  {
    return (RTOS_GET_TIME() * RTOS_MS_PER_TICK);
  }

  #define RTOS_DEFINE_STACK(name, size) TaskStack<size> __ALIGNED(8) name // stack must be aligned to 8 bytes otherwise printf for %f does not work!

  #define TASK_FUNCTION(task)           void task(void * pdata)
  #define TASK_RETURN()                 return

#else // no RTOS
  static inline void RTOS_START()
  {
  }

  static inline void RTOS_WAIT_MS(unsigned x)
  {
  }

  static inline void RTOS_WAIT_TICKS(unsigned x)
  {
  }
#endif  // RTOS type

#ifdef __cplusplus
}
#endif

#endif // _RTOS_H_
