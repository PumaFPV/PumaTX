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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DEBUG32_VALUE_COUNT 8
extern int32_t debug[DEBUG32_VALUE_COUNT];
extern uint8_t debugMode;

#define DEBUG_SET(mode, index, value) {if (debugMode == (mode)) {debug[(index)] = (value);}}

#define DEBUG_SECTION_TIMES

#ifdef DEBUG_SECTION_TIMES
#include "common/time.h"
extern timeUs_t sectionTimes[2][4];

#define TIME_SECTION_BEGIN(index) { \
    extern timeUs_t sectionTimes[2][4]; \
    sectionTimes[0][index] = micros(); \
}

#define TIME_SECTION_END(index) { \
    extern timeUs_t sectionTimes[2][4]; \
    sectionTimes[1][index] = micros(); \
    debug[index] = sectionTimes[1][index] - sectionTimes[0][index]; \
}
#else

#define TIME_SECTION_BEGIN(index) {}
#define TIME_SECTION_END(index) {}

#endif

typedef enum {
    DEBUG_NONE,
    DEBUG_GYRO,
    DEBUG_NOTCH,
    DEBUG_NAV_LANDING_DETECTOR,
    DEBUG_FW_CLIMB_RATE_TO_ALTITUDE,
    DEBUG_AGL,
    DEBUG_FLOW_RAW,
    DEBUG_FLOW,
    DEBUG_SBUS,
    DEBUG_FPORT,
    DEBUG_ALWAYS,
    DEBUG_STAGE2,
    DEBUG_WIND_ESTIMATOR,
    DEBUG_SAG_COMP_VOLTAGE,
    DEBUG_VIBE,
    DEBUG_CRUISE,
    DEBUG_REM_FLIGHT_TIME,
    DEBUG_SMARTAUDIO,
    DEBUG_ACC,
    DEBUG_GENERIC,
    DEBUG_COUNT
} debugType_e;

#if defined(USE_DEBUG_TRACE)
void debugTraceInit(void);
void debugTracePrintf(bool synchronous, const char *format, ...);
void debugTracePrintBufferHex(bool synchronous, const void *buffer, size_t size);
#define DEBUG_TRACE(fmt, ...)                   debugTracePrintf(false, fmt, ##__VA_ARGS__)
#define DEBUG_TRACE_SYNC(fmt, ...)              debugTracePrintf(true, fmt, ##__VA_ARGS__)
#define DEBUG_TRACE_BUFFER_HEX(buf, size)       debugTracePrintBufferHex(false, buf, size)
#define DEBUG_TRACE_BUFFER_HEX_SYNC(buf, size)  debugTracePrintBufferHex(true, buf, size)
#else
#define DEBUG_TRACE(fmt, ...)
#define DEBUG_TRACE_SYNC(fmt, ...)
#define DEBUG_TRACE_BUFFER_HEX(buf, size)
#define DEBUG_TRACE_BUFFER_HEX_SYNC(buf, size)
#endif
