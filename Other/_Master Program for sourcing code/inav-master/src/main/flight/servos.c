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
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#include "build/debug.h"
#include "build/build_config.h"

#include "common/axis.h"
#include "common/filter.h"
#include "common/maths.h"

#include "config/config_reset.h"
#include "config/feature.h"
#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "drivers/pwm_output.h"
#include "drivers/time.h"

#include "fc/config.h"
#include "fc/rc_controls.h"
#include "fc/rc_modes.h"
#include "fc/runtime_config.h"
#include "fc/controlrate_profile.h"

#include "flight/imu.h"
#include "flight/mixer.h"
#include "flight/pid.h"
#include "flight/servos.h"

#include "rx/rx.h"

#include "sensors/gyro.h"

PG_REGISTER_WITH_RESET_TEMPLATE(servoConfig_t, servoConfig, PG_SERVO_CONFIG, 0);

PG_RESET_TEMPLATE(servoConfig_t, servoConfig,
    .servoCenterPulse = 1500,
    .servoPwmRate = 50,             // Default for analog servos
    .servo_lowpass_freq = 20,       // Default servo update rate is 50Hz, everything above Nyquist frequency (25Hz) is going to fold and cause distortions
    .flaperon_throw_offset = FLAPERON_THROW_DEFAULT,
    .tri_unarmed_servo = 1
);

PG_REGISTER_ARRAY(servoMixer_t, MAX_SERVO_RULES, customServoMixers, PG_SERVO_MIXER, 0);

PG_REGISTER_ARRAY_WITH_RESET_FN(servoParam_t, MAX_SUPPORTED_SERVOS, servoParams, PG_SERVO_PARAMS, 2);

void pgResetFn_servoParams(servoParam_t *instance)
{
    for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
        RESET_CONFIG(servoParam_t, &instance[i],
            .min = DEFAULT_SERVO_MIN,
            .max = DEFAULT_SERVO_MAX,
            .middle = DEFAULT_SERVO_MIDDLE,
            .rate = 100
        );
    }
}

int16_t servo[MAX_SUPPORTED_SERVOS];

static uint8_t servoRuleCount = 0;
static servoMixer_t currentServoMixer[MAX_SERVO_RULES];
static int servoOutputEnabled;

static uint8_t mixerUsesServos;
static uint8_t minServoIndex;
static uint8_t maxServoIndex;

static biquadFilter_t servoFilter[MAX_SUPPORTED_SERVOS];
static bool servoFilterIsSet;

static servoMetadata_t servoMetadata[MAX_SUPPORTED_SERVOS];
static rateLimitFilter_t servoSpeedLimitFilter[MAX_SERVO_RULES];

int16_t getFlaperonDirection(uint8_t servoPin)
{
    if (servoPin == SERVO_FLAPPERON_2) {
        return -1;
    } else {
        return 1;
    }
}

/*
 * Compute scaling factor for upper and lower servo throw
 */
void servoComputeScalingFactors(uint8_t servoIndex) {
    servoMetadata[servoIndex].scaleMax = (servoParams(servoIndex)->max - servoParams(servoIndex)->middle) / 500.0f;
    servoMetadata[servoIndex].scaleMin = (servoParams(servoIndex)->middle - servoParams(servoIndex)->min) / 500.0f;
}

void servosInit(void)
{
    // give all servos a default command
    for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
        servo[i] = DEFAULT_SERVO_MIDDLE;
    }

    /*
     * load mixer
     */
    loadCustomServoMixer();

    // If there are servo rules after all, update variables
    if (servoRuleCount > 0) {
        servoOutputEnabled = 1;
        mixerUsesServos = 1;
    }

    for (uint8_t i = 0; i < MAX_SUPPORTED_SERVOS; i++)
        servoComputeScalingFactors(i);

}

void loadCustomServoMixer(void)
{
    // reset settings
    servoRuleCount = 0;
    minServoIndex = 255;
    maxServoIndex = 0;
    memset(currentServoMixer, 0, sizeof(currentServoMixer));

    // load custom mixer into currentServoMixer
    for (int i = 0; i < MAX_SERVO_RULES; i++) {
        // check if done
        if (customServoMixers(i)->rate == 0)
            break;

        if (customServoMixers(i)->targetChannel < minServoIndex) {
            minServoIndex = customServoMixers(i)->targetChannel;
        }

        if (customServoMixers(i)->targetChannel > maxServoIndex) {
            maxServoIndex = customServoMixers(i)->targetChannel;
        }

        memcpy(&currentServoMixer[i], customServoMixers(i), sizeof(servoMixer_t));
        servoRuleCount++;
    }
}

static void filterServos(void)
{
    if (servoConfig()->servo_lowpass_freq) {
        // Initialize servo lowpass filter (servos are calculated at looptime rate)
        if (!servoFilterIsSet) {
            for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
                biquadFilterInitLPF(&servoFilter[i], servoConfig()->servo_lowpass_freq, gyro.targetLooptime);
                biquadFilterReset(&servoFilter[i], servo[i]);
            }
            servoFilterIsSet = true;
        }

        for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
            // Apply servo lowpass filter and do sanity cheching
            servo[i] = (int16_t)lrintf(biquadFilterApply(&servoFilter[i], (float)servo[i]));
        }
    }

    for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
        servo[i] = constrain(servo[i], servoParams(i)->min, servoParams(i)->max);
    }
}

void writeServos(void)
{
    filterServos();

    int servoIndex = 0;
    bool zeroServoValue = false;

    /*
     * in case of tricopters, there might me a need to zero servo output when unarmed
     */
    if (mixerConfig()->platformType == PLATFORM_TRICOPTER && !ARMING_FLAG(ARMED) && !servoConfig()->tri_unarmed_servo) {
        zeroServoValue = true;
    }

    for (int i = minServoIndex; i <= maxServoIndex; i++) {
        if (zeroServoValue) {
            pwmWriteServo(servoIndex++, 0);
        } else {
            pwmWriteServo(servoIndex++, servo[i]);
        }
    }
}

void servoMixer(float dT)
{
    int16_t input[INPUT_SOURCE_COUNT]; // Range [-500:+500]

    if (FLIGHT_MODE(MANUAL_MODE)) {
        input[INPUT_STABILIZED_ROLL] = rcCommand[ROLL];
        input[INPUT_STABILIZED_PITCH] = rcCommand[PITCH];
        input[INPUT_STABILIZED_YAW] = rcCommand[YAW];
    } else {
        // Assisted modes (gyro only or gyro+acc according to AUX configuration in Gui
        input[INPUT_STABILIZED_ROLL] = axisPID[ROLL];
        input[INPUT_STABILIZED_PITCH] = axisPID[PITCH];
        input[INPUT_STABILIZED_YAW] = axisPID[YAW];

        // Reverse yaw servo when inverted in 3D mode only for multirotor and tricopter
        if (feature(FEATURE_3D) && (rcData[THROTTLE] < PWM_RANGE_MIDDLE) &&
        (mixerConfig()->platformType == PLATFORM_MULTIROTOR || mixerConfig()->platformType == PLATFORM_TRICOPTER)) {
            input[INPUT_STABILIZED_YAW] *= -1;
        }
    }

    input[INPUT_FEATURE_FLAPS] = FLIGHT_MODE(FLAPERON) ? servoConfig()->flaperon_throw_offset : 0;

    if (IS_RC_MODE_ACTIVE(BOXCAMSTAB)) {
        input[INPUT_GIMBAL_PITCH] = scaleRange(attitude.values.pitch, -900, 900, -500, +500);
        input[INPUT_GIMBAL_ROLL] = scaleRange(attitude.values.roll, -1800, 1800, -500, +500);
    } else {
        input[INPUT_GIMBAL_PITCH] = 0;
        input[INPUT_GIMBAL_ROLL] = 0;
    }

    input[INPUT_STABILIZED_THROTTLE] = motor[0] - 1000 - 500;  // Since it derives from rcCommand or mincommand and must be [-500:+500]

    // center the RC input value around the RC middle value
    // by subtracting the RC middle value from the RC input value, we get:
    // data - middle = input
    // 2000 - 1500 = +500
    // 1500 - 1500 = 0
    // 1000 - 1500 = -500
    input[INPUT_RC_ROLL]     = rcData[ROLL]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_PITCH]    = rcData[PITCH]    - PWM_RANGE_MIDDLE;
    input[INPUT_RC_YAW]      = rcData[YAW]      - PWM_RANGE_MIDDLE;
    input[INPUT_RC_THROTTLE] = rcData[THROTTLE] - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH5]      = rcData[AUX1]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH6]      = rcData[AUX2]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH7]      = rcData[AUX3]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH8]      = rcData[AUX4]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH9]      = rcData[AUX5]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH10]     = rcData[AUX6]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH11]     = rcData[AUX7]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH12]     = rcData[AUX8]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH13]     = rcData[AUX9]     - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH14]     = rcData[AUX10]    - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH15]     = rcData[AUX11]    - PWM_RANGE_MIDDLE;
    input[INPUT_RC_CH16]     = rcData[AUX12]    - PWM_RANGE_MIDDLE;

    for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {
        servo[i] = 0;
    }

    // mix servos according to rules
    for (int i = 0; i < servoRuleCount; i++) {
        const uint8_t target = currentServoMixer[i].targetChannel;
        const uint8_t from = currentServoMixer[i].inputSource;

        /*
         * Apply mixer speed limit. 1 [one] speed unit is defined as 10us/s:
         * 0 = no limiting
         * 1 = 10us/s -> full servo sweep (from 1000 to 2000) is performed in 100s
         * 10 = 100us/s -> full sweep (from 1000 to 2000)  is performed in 10s
         * 100 = 1000us/s -> full sweep in 1s
         */
        int16_t inputLimited = (int16_t) rateLimitFilterApply4(&servoSpeedLimitFilter[i], input[from], currentServoMixer[i].speed * 10, dT);

        servo[target] += ((int32_t)inputLimited * currentServoMixer[i].rate) / 100;
    }

    for (int i = 0; i < MAX_SUPPORTED_SERVOS; i++) {

        /*
         * Apply servo rate
         */
        servo[i] = ((int32_t)servoParams(i)->rate * servo[i]) / 100L;

        /*
         * Perform acumulated servo output scaling to match servo min and max values
         * Important: is servo rate is > 100%, total servo output might be bigger than
         * min/max
         */
        if (servo[i] > 0) {
            servo[i] = (int16_t) (servo[i] * servoMetadata[i].scaleMax);
        } else {
            servo[i] = (int16_t) (servo[i] * servoMetadata[i].scaleMin);
        }

        /*
         * Add a servo midpoint to the calculation
         */
        servo[i] += servoParams(i)->middle;

        /*
         * Constrain servo position to min/max to prevent servo damage
         * If servo was saturated above min/max, that means that user most probably
         * allowed the situation when smix weight sum for an output was above 100
         */
        servo[i] = constrain(servo[i], servoParams(i)->min, servoParams(i)->max);
    }
}

#define SERVO_AUTOTRIM_TIMER_MS     2000

typedef enum {
    AUTOTRIM_IDLE,
    AUTOTRIM_COLLECTING,
    AUTOTRIM_SAVE_PENDING,
    AUTOTRIM_DONE,
} servoAutotrimState_e;

void processServoAutotrim(void)
{
    static servoAutotrimState_e trimState = AUTOTRIM_IDLE;
    static timeMs_t trimStartedAt;

    static int16_t servoMiddleBackup[MAX_SUPPORTED_SERVOS];
    static int32_t servoMiddleAccum[MAX_SUPPORTED_SERVOS];
    static int32_t servoMiddleAccumCount;

    if (IS_RC_MODE_ACTIVE(BOXAUTOTRIM)) {
        switch (trimState) {
            case AUTOTRIM_IDLE:
                if (ARMING_FLAG(ARMED)) {
                    // We are activating servo trim - backup current middles and prepare to average the data
                    for (int servoIndex = SERVO_ELEVATOR; servoIndex <= MIN(SERVO_RUDDER, MAX_SUPPORTED_SERVOS); servoIndex++) {
                        servoMiddleBackup[servoIndex] = servoParams(servoIndex)->middle;
                        servoMiddleAccum[servoIndex] = 0;
                    }

                    trimStartedAt = millis();
                    servoMiddleAccumCount = 0;
                    trimState = AUTOTRIM_COLLECTING;
                }
                else {
                    break;
                }
                // Fallthru

            case AUTOTRIM_COLLECTING:
                if (ARMING_FLAG(ARMED)) {
                    servoMiddleAccumCount++;

                    for (int servoIndex = SERVO_ELEVATOR; servoIndex <= MIN(SERVO_RUDDER, MAX_SUPPORTED_SERVOS); servoIndex++) {
                        servoMiddleAccum[servoIndex] += servo[servoIndex];
                    }

                    if ((millis() - trimStartedAt) > SERVO_AUTOTRIM_TIMER_MS) {
                        for (int servoIndex = SERVO_ELEVATOR; servoIndex <= MIN(SERVO_RUDDER, MAX_SUPPORTED_SERVOS); servoIndex++) {
                            servoParamsMutable(servoIndex)->middle = servoMiddleAccum[servoIndex] / servoMiddleAccumCount;
                        }
                        trimState = AUTOTRIM_SAVE_PENDING;
                        pidResetErrorAccumulators(); //Reset Iterm since new midpoints override previously acumulated errors
                    }
                }
                else {
                    trimState = AUTOTRIM_IDLE;
                }
                break;

            case AUTOTRIM_SAVE_PENDING:
                // Wait for disarm and save to EEPROM
                if (!ARMING_FLAG(ARMED)) {
                    saveConfigAndNotify();
                    trimState = AUTOTRIM_DONE;
                }
                break;

            case AUTOTRIM_DONE:
                break;
        }
    }
    else {
        // We are deactivating servo trim - restore servo midpoints
        if (trimState == AUTOTRIM_SAVE_PENDING) {
            for (int servoIndex = SERVO_ELEVATOR; servoIndex <= MIN(SERVO_RUDDER, MAX_SUPPORTED_SERVOS); servoIndex++) {
                servoParamsMutable(servoIndex)->middle = servoMiddleBackup[servoIndex];
            }
        }

        trimState = AUTOTRIM_IDLE;
    }
}

bool isServoOutputEnabled(void)
{
    return servoOutputEnabled;
}

bool isMixerUsingServos(void)
{
    return mixerUsesServos;
}
