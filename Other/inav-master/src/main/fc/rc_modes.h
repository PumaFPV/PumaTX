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

#include <stdbool.h>

#include "common/bitarray.h"

#include "config/parameter_group.h"

#define BOXID_NONE 255

typedef enum {
    BOXARM          = 0,
    BOXANGLE        = 1,
    BOXHORIZON      = 2,
    BOXNAVALTHOLD   = 3,    // old BOXBARO
    BOXHEADINGHOLD  = 4,    // old MAG
    BOXHEADFREE     = 5,
    BOXHEADADJ      = 6,
    BOXCAMSTAB      = 7,
    BOXNAVRTH       = 8,    // old GPSHOME
    BOXNAVPOSHOLD   = 9,    // old GPSHOLD
    BOXMANUAL       = 10,
    BOXBEEPERON     = 11,
    BOXLEDLOW       = 12,
    BOXLIGHTS       = 13,
    BOXNAVLAUNCH    = 14,
    BOXOSD          = 15,
    BOXTELEMETRY    = 16,
    BOXBLACKBOX     = 17,
    BOXFAILSAFE     = 18,
    BOXNAVWP        = 19,
    BOXAIRMODE      = 20,
    BOXHOMERESET    = 21,
    BOXGCSNAV       = 22,
    BOXKILLSWITCH   = 23,   // old HEADING LOCK
    BOXSURFACE      = 24,
    BOXFLAPERON     = 25,
    BOXTURNASSIST   = 26,
    BOXAUTOTRIM     = 27,
    BOXAUTOTUNE     = 28,
    BOXCAMERA1      = 29,
    BOXCAMERA2      = 30,
    BOXCAMERA3      = 31,
    BOXOSDALT1      = 32,
    BOXOSDALT2      = 33,
    BOXOSDALT3      = 34,
    BOXNAVCRUISE    = 35,
    BOXBRAKING      = 36,
    BOXUSER1        = 37,
    BOXUSER2        = 38,
    BOXFPVANGLEMIX  = 39,
    BOXLOITERDIRCHN = 40,
    CHECKBOX_ITEM_COUNT
} boxId_e;

// type to hold enough bits for CHECKBOX_ITEM_COUNT. Struct used for value-like behavior
typedef struct boxBitmask_s { BITARRAY_DECLARE(bits, CHECKBOX_ITEM_COUNT); } boxBitmask_t;

#define MAX_MODE_ACTIVATION_CONDITION_COUNT 20

#define CHANNEL_RANGE_MIN 900
#define CHANNEL_RANGE_MAX 2100

#define CHANNEL_RANGE_STEP_WIDTH 25

#define MODE_STEP_TO_CHANNEL_VALUE(step) (CHANNEL_RANGE_MIN + CHANNEL_RANGE_STEP_WIDTH * step)
#define CHANNEL_VALUE_TO_STEP(channelValue) ((constrain(channelValue, CHANNEL_RANGE_MIN, CHANNEL_RANGE_MAX) - CHANNEL_RANGE_MIN) / CHANNEL_RANGE_STEP_WIDTH)

#define MIN_MODE_RANGE_STEP 0
#define MAX_MODE_RANGE_STEP ((CHANNEL_RANGE_MAX - CHANNEL_RANGE_MIN) / CHANNEL_RANGE_STEP_WIDTH)

#define IS_RANGE_USABLE(range) ((range)->startStep < (range)->endStep)

// steps are 25 apart
// a value of 0 corresponds to a channel value of 900 or less
// a value of 48 corresponds to a channel value of 2100 or more
// 48 steps between 900 and 1200
typedef struct channelRange_s {
    uint8_t startStep;
    uint8_t endStep;
} channelRange_t;

typedef struct modeActivationCondition_s {
    boxId_e modeId;
    uint8_t auxChannelIndex;
    channelRange_t range;
} modeActivationCondition_t;

typedef enum {
    MODE_OPERATOR_OR, // default
    MODE_OPERATOR_AND
} modeActivationOperator_e;

typedef struct modeActivationOperatorConfig_s {
    modeActivationOperator_e modeActivationOperator;
} modeActivationOperatorConfig_t;

PG_DECLARE_ARRAY(modeActivationCondition_t, MAX_MODE_ACTIVATION_CONDITION_COUNT, modeActivationConditions);
PG_DECLARE(modeActivationOperatorConfig_t, modeActivationOperatorConfig);

bool IS_RC_MODE_ACTIVE(boxId_e boxId);
void rcModeUpdate(boxBitmask_t *newState);

bool isModeActivationConditionPresent(boxId_e modeId);

bool isUsingSticksForArming(void);
bool isAirmodeActive(void);
bool isUsingNavigationModes(void);
bool isRangeActive(uint8_t auxChannelIndex, const channelRange_t *range);

void updateActivatedModes(void);
void updateUsedModeActivationConditionFlags(void);
void configureModeActivationCondition(int macIndex, boxId_e modeId, uint8_t auxChannelIndex, uint16_t startPwm, uint16_t endPwm);
