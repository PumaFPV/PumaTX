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
#include <stdlib.h>
#include <stdint.h>

#include "platform.h"

#include "blackbox/blackbox.h"

#include "build/debug.h"

#include "common/maths.h"
#include "common/axis.h"
#include "common/color.h"
#include "common/utils.h"
#include "common/filter.h"

#include "drivers/light_led.h"
#include "drivers/serial.h"
#include "drivers/time.h"
#include "drivers/system.h"
#include "drivers/pwm_output.h"

#include "sensors/sensors.h"
#include "sensors/diagnostics.h"
#include "sensors/boardalignment.h"
#include "sensors/acceleration.h"
#include "sensors/barometer.h"
#include "sensors/pitotmeter.h"
#include "sensors/gyro.h"
#include "sensors/battery.h"
#include "sensors/rangefinder.h"
#include "sensors/opflow.h"

#include "fc/fc_core.h"
#include "fc/cli.h"
#include "fc/config.h"
#include "fc/controlrate_profile.h"
#include "fc/rc_adjustments.h"
#include "fc/rc_smoothing.h"
#include "fc/rc_controls.h"
#include "fc/rc_curves.h"
#include "fc/rc_modes.h"
#include "fc/runtime_config.h"

#include "io/beeper.h"
#include "io/dashboard.h"
#include "io/gps.h"
#include "io/serial.h"
#include "io/statusindicator.h"
#include "io/asyncfatfs/asyncfatfs.h"
#include "io/piniobox.h"

#include "msp/msp_serial.h"

#include "navigation/navigation.h"

#include "rx/rx.h"
#include "rx/msp.h"

#include "scheduler/scheduler.h"

#include "telemetry/telemetry.h"

#include "flight/mixer.h"
#include "flight/servos.h"
#include "flight/pid.h"
#include "flight/imu.h"

#include "flight/failsafe.h"

#include "config/feature.h"

// June 2013     V2.2-dev

enum {
    ALIGN_GYRO = 0,
    ALIGN_ACCEL = 1,
    ALIGN_MAG = 2
};

#define GYRO_WATCHDOG_DELAY 100  // Watchdog for boards without interrupt for gyro

timeDelta_t cycleTime = 0;         // this is the number in micro second to achieve a full loop, it can differ a little and is taken into account in the PID loop
static timeUs_t flightTime = 0;

float dT;

int16_t headFreeModeHold;

uint8_t motorControlEnable = false;

static uint32_t disarmAt;     // Time of automatic disarm when "Don't spin the motors when armed" is enabled and auto_disarm_delay is nonzero

static bool isRXDataNew;
static disarmReason_t lastDisarmReason = DISARM_NONE;

bool isCalibrating(void)
{
#ifdef USE_BARO
    if (sensors(SENSOR_BARO) && !baroIsCalibrationComplete()) {
        return true;
    }
#endif

#ifdef USE_PITOT
    if (sensors(SENSOR_PITOT) && !pitotIsCalibrationComplete()) {
        return true;
    }
#endif

#ifdef USE_NAV
    if (!navIsCalibrationComplete()) {
        return true;
    }
#endif

    if (!accIsCalibrationComplete() && sensors(SENSOR_ACC)) {
        return true;
    }

    if (!gyroIsCalibrationComplete()) {
        return true;
    }

    return false;
}

int16_t getAxisRcCommand(int16_t rawData, int16_t rate, int16_t deadband)
{
    int16_t stickDeflection;

    stickDeflection = constrain(rawData - PWM_RANGE_MIDDLE, -500, 500);
    stickDeflection = applyDeadband(stickDeflection, deadband);

    return rcLookup(stickDeflection, rate);
}

static void updateArmingStatus(void)
{
    if (ARMING_FLAG(ARMED)) {
        LED0_ON;
    } else {
        /* CHECK: Run-time calibration */
        static bool calibratingFinishedBeep = false;
        if (isCalibrating()) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_SENSORS_CALIBRATING);
            calibratingFinishedBeep = false;
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_SENSORS_CALIBRATING);

            if (!calibratingFinishedBeep) {
                calibratingFinishedBeep = true;
                beeper(BEEPER_RUNTIME_CALIBRATION_DONE);
            }
        }

        /* CHECK: RX signal */
        if (!failsafeIsReceivingRxData()) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_RC_LINK);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_RC_LINK);
        }

        /* CHECK: Throttle */
        if (!armingConfig()->fixed_wing_auto_arm) {
            // Don't want this check if fixed_wing_auto_arm is in use - machine arms on throttle > LOW
            if (calculateThrottleStatus() != THROTTLE_LOW) {
                ENABLE_ARMING_FLAG(ARMING_DISABLED_THROTTLE);
            } else {
                DISABLE_ARMING_FLAG(ARMING_DISABLED_THROTTLE);
            }
        }

	/* CHECK: pitch / roll sticks centered when NAV_LAUNCH_MODE enabled */
	if (isNavLaunchEnabled()) {
	  if (areSticksDeflectedMoreThanPosHoldDeadband()) {
	    ENABLE_ARMING_FLAG(ARMING_DISABLED_ROLLPITCH_NOT_CENTERED);
	  } else {
	    DISABLE_ARMING_FLAG(ARMING_DISABLED_ROLLPITCH_NOT_CENTERED);
	  }
	}

        /* CHECK: Angle */
        if (!STATE(SMALL_ANGLE)) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_NOT_LEVEL);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_NOT_LEVEL);
        }

        /* CHECK: CPU load */
        if (isSystemOverloaded()) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_SYSTEM_OVERLOADED);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_SYSTEM_OVERLOADED);
        }

#if defined(USE_NAV)
        /* CHECK: Navigation safety */
        if (navigationBlockArming()) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_NAVIGATION_UNSAFE);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_NAVIGATION_UNSAFE);
        }
#endif

#if defined(USE_MAG)
        /* CHECK: */
        if (sensors(SENSOR_MAG) && !STATE(COMPASS_CALIBRATED)) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_COMPASS_NOT_CALIBRATED);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_COMPASS_NOT_CALIBRATED);
        }
#endif

        /* CHECK: */
        if (sensors(SENSOR_ACC) && !STATE(ACCELEROMETER_CALIBRATED)) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_ACCELEROMETER_NOT_CALIBRATED);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_ACCELEROMETER_NOT_CALIBRATED);
        }

        /* CHECK: */
        if (!isHardwareHealthy()) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_HARDWARE_FAILURE);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_HARDWARE_FAILURE);
        }

        /* CHECK: Arming switch */
        if (!isUsingSticksForArming()) {
            // If arming is disabled and the ARM switch is on
            if (isArmingDisabled() && IS_RC_MODE_ACTIVE(BOXARM)) {
                ENABLE_ARMING_FLAG(ARMING_DISABLED_ARM_SWITCH);
            } else if (!IS_RC_MODE_ACTIVE(BOXARM)) {
                DISABLE_ARMING_FLAG(ARMING_DISABLED_ARM_SWITCH);
            }
        }

        /* CHECK: BOXFAILSAFE */
        if (IS_RC_MODE_ACTIVE(BOXFAILSAFE)) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_BOXFAILSAFE);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_BOXFAILSAFE);
        }

        /* CHECK: BOXFAILSAFE */
        if (IS_RC_MODE_ACTIVE(BOXKILLSWITCH)) {
            ENABLE_ARMING_FLAG(ARMING_DISABLED_BOXKILLSWITCH);
        }
        else {
            DISABLE_ARMING_FLAG(ARMING_DISABLED_BOXKILLSWITCH);
        }
        /* CHECK: Do not allow arming if Servo AutoTrim is enabled */
        if (IS_RC_MODE_ACTIVE(BOXAUTOTRIM)) {
	    ENABLE_ARMING_FLAG(ARMING_DISABLED_SERVO_AUTOTRIM);
	    }
        else {
	    DISABLE_ARMING_FLAG(ARMING_DISABLED_SERVO_AUTOTRIM);
	    }

        if (isArmingDisabled()) {
            warningLedFlash();
        } else {
            warningLedDisable();
        }

        warningLedUpdate();
    }
}

void annexCode(void)
{
    int32_t throttleValue;

    if (failsafeShouldApplyControlInput()) {
        // Failsafe will apply rcCommand for us
        failsafeApplyControlInput();
    }
    else {
        // Compute ROLL PITCH and YAW command
        rcCommand[ROLL] = getAxisRcCommand(rcData[ROLL], FLIGHT_MODE(MANUAL_MODE) ? currentControlRateProfile->manual.rcExpo8 : currentControlRateProfile->stabilized.rcExpo8, rcControlsConfig()->deadband);
        rcCommand[PITCH] = getAxisRcCommand(rcData[PITCH], FLIGHT_MODE(MANUAL_MODE) ? currentControlRateProfile->manual.rcExpo8 : currentControlRateProfile->stabilized.rcExpo8, rcControlsConfig()->deadband);
        rcCommand[YAW] = -getAxisRcCommand(rcData[YAW], FLIGHT_MODE(MANUAL_MODE) ? currentControlRateProfile->manual.rcYawExpo8 : currentControlRateProfile->stabilized.rcYawExpo8, rcControlsConfig()->yaw_deadband);

        // Apply manual control rates
        if (FLIGHT_MODE(MANUAL_MODE)) {
            rcCommand[ROLL] = rcCommand[ROLL] * currentControlRateProfile->manual.rates[FD_ROLL] / 100L;
            rcCommand[PITCH] = rcCommand[PITCH] * currentControlRateProfile->manual.rates[FD_PITCH] / 100L;
            rcCommand[YAW] = rcCommand[YAW] * currentControlRateProfile->manual.rates[FD_YAW] / 100L;
        }

        //Compute THROTTLE command
        throttleValue = constrain(rcData[THROTTLE], rxConfig()->mincheck, PWM_RANGE_MAX);
        throttleValue = (uint32_t)(throttleValue - rxConfig()->mincheck) * PWM_RANGE_MIN / (PWM_RANGE_MAX - rxConfig()->mincheck);       // [MINCHECK;2000] -> [0;1000]
        rcCommand[THROTTLE] = rcLookupThrottle(throttleValue);

        // Signal updated rcCommand values to Failsafe system
        failsafeUpdateRcCommandValues();

        if (FLIGHT_MODE(HEADFREE_MODE)) {
            const float radDiff = degreesToRadians(DECIDEGREES_TO_DEGREES(attitude.values.yaw) - headFreeModeHold);
            const float cosDiff = cos_approx(radDiff);
            const float sinDiff = sin_approx(radDiff);
            const int16_t rcCommand_PITCH = rcCommand[PITCH] * cosDiff + rcCommand[ROLL] * sinDiff;
            rcCommand[ROLL] = rcCommand[ROLL] * cosDiff - rcCommand[PITCH] * sinDiff;
            rcCommand[PITCH] = rcCommand_PITCH;
        }
    }

    updateArmingStatus();
}

void disarm(disarmReason_t disarmReason)
{
    if (ARMING_FLAG(ARMED)) {
        lastDisarmReason = disarmReason;
        DISABLE_ARMING_FLAG(ARMED);

#ifdef USE_BLACKBOX
        if (feature(FEATURE_BLACKBOX)) {
            blackboxFinish();
        }
#endif

        statsOnDisarm();

        beeper(BEEPER_DISARMING);      // emit disarm tone
    }
}

disarmReason_t getDisarmReason(void)
{
    return lastDisarmReason;
}

#define TELEMETRY_FUNCTION_MASK (FUNCTION_TELEMETRY_FRSKY | FUNCTION_TELEMETRY_HOTT | FUNCTION_TELEMETRY_SMARTPORT | FUNCTION_TELEMETRY_LTM | FUNCTION_TELEMETRY_MAVLINK | FUNCTION_TELEMETRY_IBUS)

void releaseSharedTelemetryPorts(void) {
    serialPort_t *sharedPort = findSharedSerialPort(TELEMETRY_FUNCTION_MASK, FUNCTION_MSP);
    while (sharedPort) {
        mspSerialReleasePortIfAllocated(sharedPort);
        sharedPort = findNextSharedSerialPort(TELEMETRY_FUNCTION_MASK, FUNCTION_MSP);
    }
}

void tryArm(void)
{
    updateArmingStatus();

    if (!isArmingDisabled()) {
        if (ARMING_FLAG(ARMED)) {
            return;
        }

        lastDisarmReason = DISARM_NONE;

        ENABLE_ARMING_FLAG(ARMED);
        ENABLE_ARMING_FLAG(WAS_EVER_ARMED);
        headFreeModeHold = DECIDEGREES_TO_DEGREES(attitude.values.yaw);

        resetHeadingHoldTarget(DECIDEGREES_TO_DEGREES(attitude.values.yaw));

#ifdef USE_BLACKBOX
        if (feature(FEATURE_BLACKBOX)) {
            serialPort_t *sharedBlackboxAndMspPort = findSharedSerialPort(FUNCTION_BLACKBOX, FUNCTION_MSP);
            if (sharedBlackboxAndMspPort) {
                mspSerialReleasePortIfAllocated(sharedBlackboxAndMspPort);
            }
            blackboxStart();
        }
#endif
        disarmAt = millis() + armingConfig()->auto_disarm_delay * 1000;   // start disarm timeout, will be extended when throttle is nonzero

        //beep to indicate arming
#ifdef USE_NAV
        if (navigationPositionEstimateIsHealthy())
            beeper(BEEPER_ARMING_GPS_FIX);
        else
            beeper(BEEPER_ARMING);
#else
        beeper(BEEPER_ARMING);
#endif
        statsOnArm();

        return;
    }

    if (!ARMING_FLAG(ARMED)) {
        beeperConfirmationBeeps(1);
    }
}

void processRx(timeUs_t currentTimeUs)
{
    static bool armedBeeperOn = false;

    // Calculate RPY channel data
    calculateRxChannelsAndUpdateFailsafe(currentTimeUs);

    // in 3D mode, we need to be able to disarm by switch at any time
    if (feature(FEATURE_3D)) {
        if (!IS_RC_MODE_ACTIVE(BOXARM))
            disarm(DISARM_SWITCH_3D);
    }

    updateRSSI(currentTimeUs);

    // Update failsafe monitoring system
    if (currentTimeUs > FAILSAFE_POWER_ON_DELAY_US && !failsafeIsMonitoring()) {
        failsafeStartMonitoring();
    }

    failsafeUpdateState();

    const throttleStatus_e throttleStatus = calculateThrottleStatus();

    // When armed and motors aren't spinning, do beeps and then disarm
    // board after delay so users without buzzer won't lose fingers.
    // mixTable constrains motor commands, so checking  throttleStatus is enough
    if (ARMING_FLAG(ARMED)
        && feature(FEATURE_MOTOR_STOP)
        && !STATE(FIXED_WING)
    ) {
        if (isUsingSticksForArming()) {
            if (throttleStatus == THROTTLE_LOW) {
                if (armingConfig()->auto_disarm_delay != 0
                    && (int32_t)(disarmAt - millis()) < 0
                ) {
                    // auto-disarm configured and delay is over
                    disarm(DISARM_TIMEOUT);
                    armedBeeperOn = false;
                } else {
                    // still armed; do warning beeps while armed
                    beeper(BEEPER_ARMED);
                    armedBeeperOn = true;
                }
            } else {
                // throttle is not low
                if (armingConfig()->auto_disarm_delay != 0) {
                    // extend disarm time
                    disarmAt = millis() + armingConfig()->auto_disarm_delay * 1000;
                }

                if (armedBeeperOn) {
                    beeperSilence();
                    armedBeeperOn = false;
                }
            }
        } else {
            // arming is via AUX switch; beep while throttle low
            if (throttleStatus == THROTTLE_LOW) {
                beeper(BEEPER_ARMED);
                armedBeeperOn = true;
            } else if (armedBeeperOn) {
                beeperSilence();
                armedBeeperOn = false;
            }
        }
    }

    processRcStickPositions(throttleStatus);

    updateActivatedModes();

#ifdef USE_PINIOBOX
    pinioBoxUpdate();
#endif

    if (!cliMode) {
        bool canUseRxData = rxIsReceivingSignal() && !FLIGHT_MODE(FAILSAFE_MODE);
        updateAdjustmentStates(canUseRxData);
        processRcAdjustments(CONST_CAST(controlRateConfig_t*, currentControlRateProfile), canUseRxData);
    }

    bool canUseHorizonMode = true;

    if ((IS_RC_MODE_ACTIVE(BOXANGLE) || failsafeRequiresAngleMode() || navigationRequiresAngleMode()) && sensors(SENSOR_ACC)) {
        // bumpless transfer to Level mode
        canUseHorizonMode = false;

        if (!FLIGHT_MODE(ANGLE_MODE)) {
            ENABLE_FLIGHT_MODE(ANGLE_MODE);
        }
    } else {
        DISABLE_FLIGHT_MODE(ANGLE_MODE); // failsafe support
    }

    if (IS_RC_MODE_ACTIVE(BOXHORIZON) && canUseHorizonMode) {

        DISABLE_FLIGHT_MODE(ANGLE_MODE);

        if (!FLIGHT_MODE(HORIZON_MODE)) {
            ENABLE_FLIGHT_MODE(HORIZON_MODE);
        }
    } else {
        DISABLE_FLIGHT_MODE(HORIZON_MODE);
    }

    if (FLIGHT_MODE(ANGLE_MODE) || FLIGHT_MODE(HORIZON_MODE)) {
        LED1_ON;
    } else {
        LED1_OFF;
    }

    /* Flaperon mode */
    if (IS_RC_MODE_ACTIVE(BOXFLAPERON) && STATE(FLAPERON_AVAILABLE)) {
        if (!FLIGHT_MODE(FLAPERON)) {
            ENABLE_FLIGHT_MODE(FLAPERON);
        }
    } else {
        DISABLE_FLIGHT_MODE(FLAPERON);
    }

    /* Turn assistant mode */
    if (IS_RC_MODE_ACTIVE(BOXTURNASSIST)) {
        if (!FLIGHT_MODE(TURN_ASSISTANT)) {
            ENABLE_FLIGHT_MODE(TURN_ASSISTANT);
        }
    } else {
        DISABLE_FLIGHT_MODE(TURN_ASSISTANT);
    }

    if (sensors(SENSOR_ACC)) {
        if (IS_RC_MODE_ACTIVE(BOXHEADINGHOLD)) {
            if (!FLIGHT_MODE(HEADING_MODE)) {
                resetHeadingHoldTarget(DECIDEGREES_TO_DEGREES(attitude.values.yaw));
                ENABLE_FLIGHT_MODE(HEADING_MODE);
            }
        } else {
            DISABLE_FLIGHT_MODE(HEADING_MODE);
        }
    }

#if defined(USE_MAG)
    if (sensors(SENSOR_ACC) || sensors(SENSOR_MAG)) {
        if (IS_RC_MODE_ACTIVE(BOXHEADFREE)) {
            if (!FLIGHT_MODE(HEADFREE_MODE)) {
                ENABLE_FLIGHT_MODE(HEADFREE_MODE);
            }
        } else {
            DISABLE_FLIGHT_MODE(HEADFREE_MODE);
        }
        if (IS_RC_MODE_ACTIVE(BOXHEADADJ)) {
            headFreeModeHold = DECIDEGREES_TO_DEGREES(attitude.values.yaw); // acquire new heading
        }
    }
#endif

    // Handle passthrough mode
    if (STATE(FIXED_WING)) {
        if ((IS_RC_MODE_ACTIVE(BOXMANUAL) && !navigationRequiresAngleMode() && !failsafeRequiresAngleMode()) ||    // Normal activation of passthrough
            (!ARMING_FLAG(ARMED) && isCalibrating())){                                                              // Backup - if we are not armed - enforce passthrough while calibrating
            ENABLE_FLIGHT_MODE(MANUAL_MODE);
        } else {
            DISABLE_FLIGHT_MODE(MANUAL_MODE);
        }
    }

    /* In airmode Iterm should be prevented to grow when Low thottle and Roll + Pitch Centered.
       This is needed to prevent Iterm winding on the ground, but keep full stabilisation on 0 throttle while in air
       Low Throttle + roll and Pitch centered is assuming the copter is on the ground. Done to prevent complex air/ground detections */
    if (FLIGHT_MODE(MANUAL_MODE) || !ARMING_FLAG(ARMED)) {
        /* In MANUAL mode we reset integrators prevent I-term wind-up (PID output is not used in MANUAL) */
        pidResetErrorAccumulators();
    }
    else {
        if (throttleStatus == THROTTLE_LOW) {
            if (isAirmodeActive() && !failsafeIsActive() && ARMING_FLAG(ARMED)) {
                rollPitchStatus_e rollPitchStatus = calculateRollPitchCenterStatus();

                // ANTI_WINDUP at centred stick with MOTOR_STOP is needed on MRs and not needed on FWs
                if ((rollPitchStatus == CENTERED) || (feature(FEATURE_MOTOR_STOP) && !STATE(FIXED_WING))) {
                    ENABLE_STATE(ANTI_WINDUP);
                }
                else {
                    DISABLE_STATE(ANTI_WINDUP);
                }
            }
            else {
                DISABLE_STATE(ANTI_WINDUP);
                pidResetErrorAccumulators();
            }
        }
        else {
            DISABLE_STATE(ANTI_WINDUP);
        }
    }

    if (mixerConfig()->platformType == PLATFORM_AIRPLANE) {
        DISABLE_FLIGHT_MODE(HEADFREE_MODE);
    }

#if defined(USE_AUTOTUNE_FIXED_WING) || defined(USE_AUTOTUNE_MULTIROTOR)
    autotuneUpdateState();
#endif

#ifdef USE_TELEMETRY
    if (feature(FEATURE_TELEMETRY)) {
        if ((!telemetryConfig()->telemetry_switch && ARMING_FLAG(ARMED)) ||
                (telemetryConfig()->telemetry_switch && IS_RC_MODE_ACTIVE(BOXTELEMETRY))) {

            releaseSharedTelemetryPorts();
        } else {
            // the telemetry state must be checked immediately so that shared serial ports are released.
            telemetryCheckState();
            mspSerialAllocatePorts();
        }
    }
#endif

}

// Function for loop trigger
void taskGyro(timeUs_t currentTimeUs) {
    // getTaskDeltaTime() returns delta time frozen at the moment of entering the scheduler. currentTime is frozen at the very same point.
    // To make busy-waiting timeout work we need to account for time spent within busy-waiting loop
    const timeDelta_t currentDeltaTime = getTaskDeltaTime(TASK_SELF);
    timeUs_t gyroUpdateUs = currentTimeUs;

    if (gyroConfig()->gyroSync) {
        while (true) {
            gyroUpdateUs = micros();
            if (gyroSyncCheckUpdate() || ((currentDeltaTime + cmpTimeUs(gyroUpdateUs, currentTimeUs)) >= (timeDelta_t)(getLooptime() + GYRO_WATCHDOG_DELAY))) {
                break;
            }
        }
    }

    /* Update actual hardware readings */
    gyroUpdate();

#ifdef USE_OPTICAL_FLOW
    if (sensors(SENSOR_OPFLOW)) {
        opflowGyroUpdateCallback((timeUs_t)currentDeltaTime + (gyroUpdateUs - currentTimeUs));
    }
#endif
}

static float calculateThrottleTiltCompensationFactor(uint8_t throttleTiltCompensationStrength)
{
    if (throttleTiltCompensationStrength) {
        float tiltCompFactor = 1.0f / constrainf(calculateCosTiltAngle(), 0.6f, 1.0f);  // max tilt about 50 deg
        return 1.0f + (tiltCompFactor - 1.0f) * (throttleTiltCompensationStrength / 100.f);
    } else {
        return 1.0f;
    }
}

void taskMainPidLoop(timeUs_t currentTimeUs)
{
    cycleTime = getTaskDeltaTime(TASK_SELF);
    dT = (float)cycleTime * 0.000001f;

    if (ARMING_FLAG(ARMED) && (!STATE(FIXED_WING) || !isNavLaunchEnabled() || (isNavLaunchEnabled() && (isFixedWingLaunchDetected() || isFixedWingLaunchFinishedOrAborted())))) {
        flightTime += cycleTime;
    }

    taskGyro(currentTimeUs);
    imuUpdateAccelerometer();
    imuUpdateAttitude(currentTimeUs);

    annexCode();

    if (rxConfig()->rcFilterFrequency) {
        rcInterpolationApply(isRXDataNew);
    }

#if defined(USE_NAV)
    if (isRXDataNew) {
        updateWaypointsAndNavigationMode();
    }
#endif

    isRXDataNew = false;

#if defined(USE_NAV)
    updatePositionEstimator();
    applyWaypointNavigationAndAltitudeHold();
#endif

    // If we're armed, at minimum throttle, and we do arming via the
    // sticks, do not process yaw input from the rx.  We do this so the
    // motors do not spin up while we are trying to arm or disarm.
    // Allow yaw control for tricopters if the user wants the servo to move even when unarmed.
    if (isUsingSticksForArming() && rcData[THROTTLE] <= rxConfig()->mincheck
            && !((mixerConfig()->platformType == PLATFORM_TRICOPTER) && servoConfig()->tri_unarmed_servo)
            && mixerConfig()->platformType != PLATFORM_AIRPLANE
    ) {
        rcCommand[YAW] = 0;
    }

    // Apply throttle tilt compensation
    if (!STATE(FIXED_WING)) {
        int16_t thrTiltCompStrength = 0;

        if (navigationRequiresThrottleTiltCompensation()) {
            thrTiltCompStrength = 100;
        }
        else if (systemConfig()->throttle_tilt_compensation_strength && (FLIGHT_MODE(ANGLE_MODE) || FLIGHT_MODE(HORIZON_MODE))) {
            thrTiltCompStrength = systemConfig()->throttle_tilt_compensation_strength;
        }

        if (thrTiltCompStrength) {
            rcCommand[THROTTLE] = constrain(motorConfig()->minthrottle
                                            + (rcCommand[THROTTLE] - motorConfig()->minthrottle) * calculateThrottleTiltCompensationFactor(thrTiltCompStrength),
                                            motorConfig()->minthrottle,
                                            motorConfig()->maxthrottle);
        }
    }
    else {
        // FIXME: throttle pitch comp for FW
    }

    // Update PID coefficients
    updatePIDCoefficients();

    // Calculate stabilisation
    pidController();

#ifdef HIL
    if (hilActive) {
        hilUpdateControlState();
        motorControlEnable = false;
    }
#endif

    mixTable(dT);

    if (isMixerUsingServos()) {
        servoMixer(dT);
        processServoAutotrim();
    }

    //Servos should be filtered or written only when mixer is using servos or special feaures are enabled
    if (isServoOutputEnabled()) {
        writeServos();
    }

    if (motorControlEnable) {
        writeMotors();
    }

#ifdef USE_BLACKBOX
    if (!cliMode && feature(FEATURE_BLACKBOX)) {
        blackboxUpdate(micros());
    }
#endif
}

// This function is called in a busy-loop, everything called from here should do it's own
// scheduling and avoid doing heavy calculations
void taskRunRealtimeCallbacks(timeUs_t currentTimeUs)
{
    UNUSED(currentTimeUs);

#ifdef USE_SDCARD
    afatfs_poll();
#endif

#ifdef USE_DSHOT
    pwmCompleteDshotUpdate(getMotorCount());
#endif
}

bool taskUpdateRxCheck(timeUs_t currentTimeUs, timeDelta_t currentDeltaTime)
{
    UNUSED(currentDeltaTime);

    return rxUpdateCheck(currentTimeUs, currentDeltaTime);
}

void taskUpdateRxMain(timeUs_t currentTimeUs)
{
    processRx(currentTimeUs);
    isRXDataNew = true;
}

// returns seconds
float getFlightTime()
{
    return (float)(flightTime / 1000) / 1000;
}

void fcReboot(bool bootLoader)
{
    // stop motor/servo outputs
    stopMotors();
    stopPwmAllMotors();

    // extra delay before reboot to give ESCs chance to reset
    delay(1000);

    if (bootLoader) {
        systemResetToBootloader();
    }
    else {
        systemReset();
    }

    while (true);
}
