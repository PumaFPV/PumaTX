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
#include <stdint.h>
#include <string.h>

#include "rc_modes.h"

#include "common/bitarray.h"
#include "common/maths.h"
#include "common/utils.h"

#include "config/feature.h"
#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "fc/config.h"
#include "fc/rc_controls.h"

#include "rx/rx.h"

static uint8_t specifiedConditionCountPerMode[CHECKBOX_ITEM_COUNT];
static bool isUsingSticksToArm = true;
#ifdef USE_NAV
static bool isUsingNAVModes = false;
#endif

boxBitmask_t rcModeActivationMask; // one bit per mode defined in boxId_e

// TODO(alberto): It looks like we can now safely remove this assert, since everything
// but BB is able to handle more than 32 boxes and all the definitions use
// CHECKBOX_ITEM_COUNT rather than hardcoded values. Note, however, that BB will only
// log the first 32 flight modes, so the ones affecting actual flight should be <= 32.
//
// Leaving the assert commented for now, just in case there are some unexpected issues
// and someone else has to debug it.
// STATIC_ASSERT(CHECKBOX_ITEM_COUNT <= 32, too_many_box_modes);

PG_REGISTER_ARRAY(modeActivationCondition_t, MAX_MODE_ACTIVATION_CONDITION_COUNT, modeActivationConditions, PG_MODE_ACTIVATION_PROFILE, 0);
PG_REGISTER(modeActivationOperatorConfig_t, modeActivationOperatorConfig, PG_MODE_ACTIVATION_OPERATOR_CONFIG, 0);

bool isUsingSticksForArming(void)
{
    return isUsingSticksToArm;
}

bool isAirmodeActive(void)
{
    return feature(FEATURE_AIRMODE) || IS_RC_MODE_ACTIVE(BOXAIRMODE);
}

#if defined(USE_NAV)
bool isUsingNavigationModes(void)
{
    return isUsingNAVModes;
}
#endif


bool IS_RC_MODE_ACTIVE(boxId_e boxId)
{
    return bitArrayGet(rcModeActivationMask.bits, boxId);
}

void rcModeUpdate(boxBitmask_t *newState)
{
    rcModeActivationMask = *newState;
}

bool isModeActivationConditionPresent(boxId_e modeId)
{
    for (int index = 0; index < MAX_MODE_ACTIVATION_CONDITION_COUNT; index++) {
        if (modeActivationConditions(index)->modeId == modeId && IS_RANGE_USABLE(&modeActivationConditions(index)->range)) {
            return true;
        }
    }

    return false;
}

bool isRangeActive(uint8_t auxChannelIndex, const channelRange_t *range)
{
    if (!IS_RANGE_USABLE(range)) {
        return false;
    }

    // No need to constrain() here, since we're testing for a closed range defined
    // by the channelRange_t. If channelValue has an invalid value, the test will
    // be false anyway.
    uint16_t channelValue = rcData[auxChannelIndex + NON_AUX_CHANNEL_COUNT];
    return (channelValue >= CHANNEL_RANGE_MIN + (range->startStep * CHANNEL_RANGE_STEP_WIDTH) &&
            channelValue < CHANNEL_RANGE_MIN + (range->endStep * CHANNEL_RANGE_STEP_WIDTH));
}

void updateActivatedModes(void)
{
    // Disable all modes to begin with
    boxBitmask_t newMask;
    memset(&newMask, 0, sizeof(newMask));

    // Unfortunately for AND logic it's not enough to simply check if any of the specified channel range conditions are valid for a mode.
    // We need to count the total number of conditions specified for each mode, and check that all those conditions are currently valid.
    uint8_t activeConditionCountPerMode[CHECKBOX_ITEM_COUNT];
    memset(activeConditionCountPerMode, 0, CHECKBOX_ITEM_COUNT);

    for (int index = 0; index < MAX_MODE_ACTIVATION_CONDITION_COUNT; index++) {
        if (isRangeActive(modeActivationConditions(index)->auxChannelIndex, &modeActivationConditions(index)->range)) {
            // Increment the number of valid conditions for this mode
            activeConditionCountPerMode[modeActivationConditions(index)->modeId]++;
        }
    }

    // Now see which modes should be enabled
    for (int modeIndex = 0; modeIndex < CHECKBOX_ITEM_COUNT; modeIndex++) {
        // only modes with conditions specified are considered
        if (specifiedConditionCountPerMode[modeIndex] > 0) {
            // For AND logic, the specified condition count and valid condition count must be the same.
            // For OR logic, the valid condition count must be greater than zero.

            if (modeActivationOperatorConfig()->modeActivationOperator == MODE_OPERATOR_AND) {
                // AND the conditions
                if (activeConditionCountPerMode[modeIndex] == specifiedConditionCountPerMode[modeIndex]) {
                    bitArraySet(newMask.bits, modeIndex);
                }
            }
            else {
                // OR the conditions
                if (activeConditionCountPerMode[modeIndex] > 0) {
                    bitArraySet(newMask.bits, modeIndex);
                }
            }
        }
    }

    rcModeUpdate(&newMask);
}

void updateUsedModeActivationConditionFlags(void)
{
    memset(specifiedConditionCountPerMode, 0, CHECKBOX_ITEM_COUNT);
    for (int index = 0; index < MAX_MODE_ACTIVATION_CONDITION_COUNT; index++) {
        if (IS_RANGE_USABLE(&modeActivationConditions(index)->range)) {
            specifiedConditionCountPerMode[modeActivationConditions(index)->modeId]++;
        }
    }

    isUsingSticksToArm = !isModeActivationConditionPresent(BOXARM);

#ifdef USE_NAV
    isUsingNAVModes = isModeActivationConditionPresent(BOXNAVPOSHOLD) ||
                        isModeActivationConditionPresent(BOXNAVRTH) ||
                        isModeActivationConditionPresent(BOXNAVCRUISE) ||
                        isModeActivationConditionPresent(BOXNAVWP);
#endif
}

void configureModeActivationCondition(int macIndex, boxId_e modeId, uint8_t auxChannelIndex, uint16_t startPwm, uint16_t endPwm)
{
    modeActivationConditionsMutable(macIndex)->modeId = modeId;
    modeActivationConditionsMutable(macIndex)->auxChannelIndex = auxChannelIndex;
    modeActivationConditionsMutable(macIndex)->range.startStep = CHANNEL_VALUE_TO_STEP(startPwm);
    modeActivationConditionsMutable(macIndex)->range.endStep = CHANNEL_VALUE_TO_STEP(endPwm);
}
