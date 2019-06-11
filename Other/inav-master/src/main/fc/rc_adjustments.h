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

#pragma once

#include "config/parameter_group.h"

#include "fc/rc_controls.h"
#include "fc/rc_modes.h"

//#define USE_INFLIGHT_PROFILE_ADJUSTMENT - not currently enabled

typedef enum {
    ADJUSTMENT_NONE                     = 0,
    ADJUSTMENT_RC_RATE                  = 1,
    ADJUSTMENT_RC_EXPO                  = 2,
    ADJUSTMENT_THROTTLE_EXPO            = 3,
    ADJUSTMENT_PITCH_ROLL_RATE          = 4,
    ADJUSTMENT_YAW_RATE                 = 5,
    ADJUSTMENT_PITCH_ROLL_P             = 6,
    ADJUSTMENT_PITCH_ROLL_I             = 7,
    ADJUSTMENT_PITCH_ROLL_D             = 8,
    ADJUSTMENT_YAW_P                    = 9,
    ADJUSTMENT_YAW_I                    = 10,
    ADJUSTMENT_YAW_D                    = 11,
    ADJUSTMENT_RATE_PROFILE             = 12,    // Unused, placeholder for compatibility
    ADJUSTMENT_PITCH_RATE               = 13,
    ADJUSTMENT_ROLL_RATE                = 14,
    ADJUSTMENT_PITCH_P                  = 15,
    ADJUSTMENT_PITCH_I                  = 16,
    ADJUSTMENT_PITCH_D                  = 17,
    ADJUSTMENT_ROLL_P                   = 18,
    ADJUSTMENT_ROLL_I                   = 19,
    ADJUSTMENT_ROLL_D                   = 20,
    ADJUSTMENT_RC_YAW_EXPO              = 21,
    ADJUSTMENT_MANUAL_RC_EXPO           = 22,
    ADJUSTMENT_MANUAL_RC_YAW_EXPO       = 23,
    ADJUSTMENT_MANUAL_PITCH_ROLL_RATE   = 24,
    ADJUSTMENT_MANUAL_ROLL_RATE         = 25,
    ADJUSTMENT_MANUAL_PITCH_RATE        = 26,
    ADJUSTMENT_MANUAL_YAW_RATE          = 27,
    ADJUSTMENT_NAV_FW_CRUISE_THR        = 28,
    ADJUSTMENT_NAV_FW_PITCH2THR         = 29,
    ADJUSTMENT_ROLL_BOARD_ALIGNMENT     = 30,
    ADJUSTMENT_PITCH_BOARD_ALIGNMENT    = 31,
    ADJUSTMENT_LEVEL_P                  = 32,
    ADJUSTMENT_LEVEL_I                  = 33,
    ADJUSTMENT_LEVEL_D                  = 34,
    ADJUSTMENT_POS_XY_P                 = 35,
    ADJUSTMENT_POS_XY_I                 = 36,
    ADJUSTMENT_POS_XY_D                 = 37,
    ADJUSTMENT_POS_Z_P                  = 38,
    ADJUSTMENT_POS_Z_I                  = 39,
    ADJUSTMENT_POS_Z_D                  = 40,
    ADJUSTMENT_HEADING_P                = 41,
    ADJUSTMENT_VEL_XY_P                 = 42,
    ADJUSTMENT_VEL_XY_I                 = 43,
    ADJUSTMENT_VEL_XY_D                 = 44,
    ADJUSTMENT_VEL_Z_P                  = 45,
    ADJUSTMENT_VEL_Z_I                  = 46,
    ADJUSTMENT_VEL_Z_D                  = 47,
    ADJUSTMENT_FW_MIN_THROTTLE_DOWN_PITCH_ANGLE = 48,
#ifdef USE_INFLIGHT_PROFILE_ADJUSTMENT
    ADJUSTMENT_PROFILE                  = 49,
#endif
    ADJUSTMENT_FUNCTION_COUNT // must be last
} adjustmentFunction_e;

typedef enum {
    ADJUSTMENT_MODE_STEP,
    ADJUSTMENT_MODE_SELECT
} adjustmentMode_e;

typedef struct adjustmentStepConfig_s {
    uint8_t step;
} adjustmentStepConfig_t;

typedef struct adjustmentSelectConfig_s {
    uint8_t switchPositions;
} adjustmentSelectConfig_t;

typedef union adjustmentConfig_u {
    adjustmentStepConfig_t stepConfig;
    adjustmentSelectConfig_t selectConfig;
} adjustmentData_t;

typedef struct adjustmentConfig_s {
    uint8_t adjustmentFunction;
    uint8_t mode;
    adjustmentData_t data;
} adjustmentConfig_t;

typedef struct adjustmentRange_s {
    // when aux channel is in range...
    channelRange_t range;
    uint8_t auxChannelIndex;

    // ..then apply the adjustment function to the auxSwitchChannel ...
    uint8_t adjustmentFunction;
    uint8_t auxSwitchChannelIndex;

    // ... via slot
    uint8_t adjustmentIndex;
} adjustmentRange_t;

#define ADJUSTMENT_INDEX_OFFSET 1

typedef struct adjustmentState_s {
    const adjustmentConfig_t *config;
    uint32_t timeoutAt;
    uint8_t auxChannelIndex;
} adjustmentState_t;


#ifndef MAX_SIMULTANEOUS_ADJUSTMENT_COUNT
#define MAX_SIMULTANEOUS_ADJUSTMENT_COUNT 4 // enough for 4 x 3position switches / 4 aux channel
#endif

#define MAX_ADJUSTMENT_RANGE_COUNT 12 // enough for 2 * 6pos switches.

PG_DECLARE_ARRAY(adjustmentRange_t, MAX_ADJUSTMENT_RANGE_COUNT, adjustmentRanges);

void resetAdjustmentStates(void);
void updateAdjustmentStates(bool canUseRxData);
struct controlRateConfig_s;
void processRcAdjustments(struct controlRateConfig_s *controlRateConfig, bool canUseRxData);
bool isAdjustmentFunctionSelected(uint8_t adjustmentFunction);
