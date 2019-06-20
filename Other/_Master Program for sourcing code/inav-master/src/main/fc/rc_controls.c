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

#include <math.h>

#include "platform.h"

#include "build/debug.h"

#include "blackbox/blackbox.h"

#include "common/axis.h"
#include "common/maths.h"
#include "common/utils.h"

#include "config/feature.h"
#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "drivers/time.h"

#include "fc/config.h"
#include "fc/controlrate_profile.h"
#include "fc/fc_core.h"
#include "fc/rc_controls.h"
#include "fc/rc_curves.h"
#include "fc/rc_modes.h"
#include "fc/runtime_config.h"

#include "flight/pid.h"
#include "flight/failsafe.h"

#include "io/gps.h"
#include "io/beeper.h"

#include "navigation/navigation.h"

#include "rx/rx.h"

#include "sensors/barometer.h"
#include "sensors/battery.h"
#include "sensors/sensors.h"
#include "sensors/gyro.h"
#include "sensors/acceleration.h"

#define AIRMODE_DEADBAND 25
#define MIN_RC_TICK_INTERVAL_MS             20
#define DEFAULT_RC_SWITCH_DISARM_DELAY_MS   250     // Wait at least 250ms before disarming via switch

stickPositions_e rcStickPositions;

int16_t rcCommand[4];           // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW

PG_REGISTER_WITH_RESET_TEMPLATE(rcControlsConfig_t, rcControlsConfig, PG_RC_CONTROLS_CONFIG, 0);

PG_RESET_TEMPLATE(rcControlsConfig_t, rcControlsConfig,
    .deadband = 5,
    .yaw_deadband = 5,
    .pos_hold_deadband = 20,
    .alt_hold_deadband = 50,
    .deadband3d_throttle = 50
);

PG_REGISTER_WITH_RESET_TEMPLATE(armingConfig_t, armingConfig, PG_ARMING_CONFIG, 1);

PG_RESET_TEMPLATE(armingConfig_t, armingConfig,
    .fixed_wing_auto_arm = 0,
    .disarm_kill_switch = 1,
    .auto_disarm_delay = 5,
    .switchDisarmDelayMs = DEFAULT_RC_SWITCH_DISARM_DELAY_MS,
);

bool areSticksInApModePosition(uint16_t ap_mode)
{
    return ABS(rcCommand[ROLL]) < ap_mode && ABS(rcCommand[PITCH]) < ap_mode;
}

bool areSticksDeflectedMoreThanPosHoldDeadband(void)
{
    return (ABS(rcCommand[ROLL]) > rcControlsConfig()->pos_hold_deadband) || (ABS(rcCommand[PITCH]) > rcControlsConfig()->pos_hold_deadband);
}

throttleStatus_e calculateThrottleStatus(void)
{
    const uint16_t deadband3d_throttle = rcControlsConfig()->deadband3d_throttle;
    if (feature(FEATURE_3D) && (rcData[THROTTLE] > (PWM_RANGE_MIDDLE - deadband3d_throttle) && rcData[THROTTLE] < (PWM_RANGE_MIDDLE + deadband3d_throttle)))
        return THROTTLE_LOW;
    else if (!feature(FEATURE_3D) && (rcData[THROTTLE] < rxConfig()->mincheck))
        return THROTTLE_LOW;

    return THROTTLE_HIGH;
}

rollPitchStatus_e calculateRollPitchCenterStatus(void)
{
    if (((rcData[PITCH] < (PWM_RANGE_MIDDLE + AIRMODE_DEADBAND)) && (rcData[PITCH] > (PWM_RANGE_MIDDLE -AIRMODE_DEADBAND)))
            && ((rcData[ROLL] < (PWM_RANGE_MIDDLE + AIRMODE_DEADBAND)) && (rcData[ROLL] > (PWM_RANGE_MIDDLE -AIRMODE_DEADBAND))))
        return CENTERED;

    return NOT_CENTERED;
}

stickPositions_e getRcStickPositions(void)
{
    return rcStickPositions;
}

bool checkStickPosition(stickPositions_e stickPos)
{
    const uint8_t mask[4] = { ROL_LO | ROL_HI, PIT_LO | PIT_HI, YAW_LO | YAW_HI, THR_LO | THR_HI };
    for (int i = 0; i < 4; i++) {
        if (((stickPos & mask[i]) != 0) && ((stickPos & mask[i]) != (rcStickPositions & mask[i]))) {
            return false;
        }
    }

    return true;
}

static void updateRcStickPositions(void)
{
    stickPositions_e tmp = 0;

    tmp |= ((rcData[ROLL] > rxConfig()->mincheck) ? 0x02 : 0x00) << (ROLL * 2);
    tmp |= ((rcData[ROLL] < rxConfig()->maxcheck) ? 0x01 : 0x00) << (ROLL * 2);

    tmp |= ((rcData[PITCH] > rxConfig()->mincheck) ? 0x02 : 0x00) << (PITCH * 2);
    tmp |= ((rcData[PITCH] < rxConfig()->maxcheck) ? 0x01 : 0x00) << (PITCH * 2);

    tmp |= ((rcData[YAW] > rxConfig()->mincheck) ? 0x02 : 0x00) << (YAW * 2);
    tmp |= ((rcData[YAW] < rxConfig()->maxcheck) ? 0x01 : 0x00) << (YAW * 2);

    tmp |= ((rcData[THROTTLE] > rxConfig()->mincheck) ? 0x02 : 0x00) << (THROTTLE * 2);
    tmp |= ((rcData[THROTTLE] < rxConfig()->maxcheck) ? 0x01 : 0x00) << (THROTTLE * 2);

    rcStickPositions = tmp;
}

void processRcStickPositions(throttleStatus_e throttleStatus)
{
    static timeMs_t lastTickTimeMs = 0;
    static uint8_t rcDelayCommand;      // this indicates the number of time (multiple of RC measurement at 50Hz) the sticks must be maintained to run or switch off motors
    static uint32_t rcSticks;           // this hold sticks position for command combos
    static timeMs_t rcDisarmTimeMs;     // this is an extra guard for disarming through switch to prevent that one frame can disarm it
    const timeMs_t currentTimeMs = millis();

    updateRcStickPositions();

    uint32_t stTmp = getRcStickPositions();
    if (stTmp == rcSticks) {
        if (rcDelayCommand < 250) {
            if ((currentTimeMs - lastTickTimeMs) >= MIN_RC_TICK_INTERVAL_MS) {
                lastTickTimeMs = currentTimeMs;
                rcDelayCommand++;
            }
        }
    } else
        rcDelayCommand = 0;

    rcSticks = stTmp;

    // perform actions
    if (!isUsingSticksForArming()) {
        if (IS_RC_MODE_ACTIVE(BOXARM)) {
            rcDisarmTimeMs = currentTimeMs;
            tryArm();
        } else {
            // Disarming via ARM BOX
            // Don't disarm via switch if failsafe is active or receiver doesn't receive data - we can't trust receiver
            // and can't afford to risk disarming in the air
            if (ARMING_FLAG(ARMED) && !IS_RC_MODE_ACTIVE(BOXFAILSAFE) && rxIsReceivingSignal() && !failsafeIsActive()) {
                const timeMs_t disarmDelay = currentTimeMs - rcDisarmTimeMs;
                if (disarmDelay > armingConfig()->switchDisarmDelayMs) {
                    if (armingConfig()->disarm_kill_switch || (throttleStatus == THROTTLE_LOW)) {
                        disarm(DISARM_SWITCH);
                    }
                }
            }
            else {
                rcDisarmTimeMs = currentTimeMs;
            }
        }
    }

    // KILLSWITCH disarms instantly
    if (IS_RC_MODE_ACTIVE(BOXKILLSWITCH)) {
        disarm(DISARM_KILLSWITCH);
    }

    if (rcDelayCommand != 20) {
        return;
    }

   if (isUsingSticksForArming()) {
        // Disarm on throttle down + yaw
        if (rcSticks == THR_LO + YAW_LO + PIT_CE + ROL_CE) {
            // Dont disarm if fixedwing and motorstop
            if (STATE(FIXED_WING) && feature(FEATURE_MOTOR_STOP) && armingConfig()->fixed_wing_auto_arm) {
                return;
            }
            else if (ARMING_FLAG(ARMED)) {
                disarm(DISARM_STICKS);
            }
            else {
                beeper(BEEPER_DISARM_REPEAT);    // sound tone while stick held
                rcDelayCommand = 0;              // reset so disarm tone will repeat
            }
        }
   }

    if (ARMING_FLAG(ARMED)) {
        // actions during armed
        return;
    }

    // actions during not armed

    // GYRO calibration
    if (rcSticks == THR_LO + YAW_LO + PIT_LO + ROL_CE) {
        gyroStartCalibration();
        return;
    }


#if defined(NAV_NON_VOLATILE_WAYPOINT_STORAGE)
    // Save waypoint list
    if (rcSticks == THR_LO + YAW_CE + PIT_HI + ROL_LO) {
        const bool success = saveNonVolatileWaypointList();
        beeper(success ? BEEPER_ACTION_SUCCESS : BEEPER_ACTION_FAIL);
    }

    // Load waypoint list
    if (rcSticks == THR_LO + YAW_CE + PIT_HI + ROL_HI) {
        const bool success = loadNonVolatileWaypointList();
        beeper(success ? BEEPER_ACTION_SUCCESS : BEEPER_ACTION_FAIL);
    }
#endif

    // Multiple configuration profiles
    if (feature(FEATURE_TX_PROF_SEL)) {

        uint8_t i = 0;

        if (rcSticks == THR_LO + YAW_LO + PIT_CE + ROL_LO)          // ROLL left  -> Profile 1
            i = 1;
        else if (rcSticks == THR_LO + YAW_LO + PIT_HI + ROL_CE)     // PITCH up   -> Profile 2
            i = 2;
        else if (rcSticks == THR_LO + YAW_LO + PIT_CE + ROL_HI)     // ROLL right -> Profile 3
            i = 3;

        if (i) {
            setConfigProfileAndWriteEEPROM(i - 1);
            return;
        }

        i = 0;

        // Multiple battery configuration profiles
        if (rcSticks == THR_HI + YAW_LO + PIT_CE + ROL_LO)          // ROLL left  -> Profile 1
            i = 1;
        else if (rcSticks == THR_HI + YAW_LO + PIT_HI + ROL_CE)     // PITCH up   -> Profile 2
            i = 2;
        else if (rcSticks == THR_HI + YAW_LO + PIT_CE + ROL_HI)     // ROLL right -> Profile 3
            i = 3;

        if (i) {
            setConfigBatteryProfileAndWriteEEPROM(i - 1);
            batteryDisableProfileAutoswitch();
            activateBatteryProfile();
            return;
        }

    }

    // Save config
    if (rcSticks == THR_LO + YAW_LO + PIT_LO + ROL_HI) {
        saveConfigAndNotify();
    }


    // Arming by sticks
    if (isUsingSticksForArming()) {
        if (STATE(FIXED_WING) && feature(FEATURE_MOTOR_STOP) && armingConfig()->fixed_wing_auto_arm) {
            // Auto arm on throttle when using fixedwing and motorstop
            if (throttleStatus != THROTTLE_LOW) {
                tryArm();
                return;
            }
        }
        else {
            if (rcSticks == THR_LO + YAW_HI + PIT_CE + ROL_CE) {
                // Arm via YAW
                tryArm();
                return;
            }
        }
    }


    // Calibrating Acc
    if (rcSticks == THR_HI + YAW_LO + PIT_LO + ROL_CE) {
        accStartCalibration();
        return;
    }


    // Calibrating Mag
    if (rcSticks == THR_HI + YAW_HI + PIT_LO + ROL_CE) {
        ENABLE_STATE(CALIBRATE_MAG);
        return;
    }


    // Accelerometer Trim
    if (rcSticks == THR_HI + YAW_CE + PIT_HI + ROL_CE) {
        applyAndSaveBoardAlignmentDelta(0, -2);
        rcDelayCommand = 10;
        return;
    } else if (rcSticks == THR_HI + YAW_CE + PIT_LO + ROL_CE) {
        applyAndSaveBoardAlignmentDelta(0, 2);
        rcDelayCommand = 10;
        return;
    } else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_HI) {
        applyAndSaveBoardAlignmentDelta(-2, 0);
        rcDelayCommand = 10;
        return;
    } else if (rcSticks == THR_HI + YAW_CE + PIT_CE + ROL_LO) {
        applyAndSaveBoardAlignmentDelta(2, 0);
        rcDelayCommand = 10;
        return;
    }
}

int32_t getRcStickDeflection(int32_t axis) {
    return MIN(ABS(rcData[axis] - PWM_RANGE_MIDDLE), 500);
}
