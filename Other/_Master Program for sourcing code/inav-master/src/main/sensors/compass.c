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
#include <math.h>

#include "platform.h"
#include "build/debug.h"

#include "common/axis.h"
#include "common/maths.h"
#include "common/utils.h"

#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "drivers/compass/compass.h"
#include "drivers/compass/compass_ak8963.h"
#include "drivers/compass/compass_ak8975.h"
#include "drivers/compass/compass_fake.h"
#include "drivers/compass/compass_hmc5883l.h"
#include "drivers/compass/compass_mag3110.h"
#include "drivers/compass/compass_ist8310.h"
#include "drivers/compass/compass_ist8308.h"
#include "drivers/compass/compass_qmc5883l.h"
#include "drivers/compass/compass_mpu9250.h"
#include "drivers/compass/compass_lis3mdl.h"
#include "drivers/io.h"
#include "drivers/light_led.h"
#include "drivers/logging.h"
#include "drivers/time.h"

#include "fc/config.h"
#include "fc/runtime_config.h"

#include "io/gps.h"
#include "io/beeper.h"

#include "sensors/boardalignment.h"
#include "sensors/compass.h"
#include "sensors/gyro.h"
#include "sensors/sensors.h"

mag_t mag;                   // mag access functions

PG_REGISTER_WITH_RESET_TEMPLATE(compassConfig_t, compassConfig, PG_COMPASS_CONFIG, 3);

#ifdef USE_MAG
#define MAG_HARDWARE_DEFAULT    MAG_AUTODETECT
#else
#define MAG_HARDWARE_DEFAULT    MAG_NONE
#endif
PG_RESET_TEMPLATE(compassConfig_t, compassConfig,
    .mag_align = ALIGN_DEFAULT,
    .mag_hardware = MAG_HARDWARE_DEFAULT,
    .mag_declination = 0,
    .mag_to_use = 0,
    .magCalibrationTimeLimit = 30,
    .rollDeciDegrees = 0,
    .pitchDeciDegrees = 0,
    .yawDeciDegrees = 0,
);

#ifdef USE_MAG

static uint8_t magUpdatedAtLeastOnce = 0;

bool compassDetect(magDev_t *dev, magSensor_e magHardwareToUse)
{
    magSensor_e magHardware = MAG_NONE;
    requestedSensors[SENSOR_INDEX_MAG] = magHardwareToUse;

    dev->magAlign.useExternal = false;
    dev->magAlign.onBoard = ALIGN_DEFAULT;

    switch (magHardwareToUse) {
    case MAG_AUTODETECT:
        FALLTHROUGH;

    case MAG_QMC5883:
#ifdef USE_MAG_QMC5883
        if (qmc5883Detect(dev)) {
#ifdef MAG_QMC5883_ALIGN
            dev->magAlign.onBoard = MAG_QMC5883_ALIGN;
#endif
            magHardware = MAG_QMC5883;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_HMC5883:
#ifdef USE_MAG_HMC5883
        if (hmc5883lDetect(dev)) {
#ifdef MAG_HMC5883_ALIGN
            dev->magAlign.onBoard = MAG_HMC5883_ALIGN;
#endif
            magHardware = MAG_HMC5883;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_AK8975:
#ifdef USE_MAG_AK8975
        if (ak8975Detect(dev)) {
#ifdef MAG_AK8975_ALIGN
            dev->magAlign.onBoard = MAG_AK8975_ALIGN;
#endif
            magHardware = MAG_AK8975;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_AK8963:
#ifdef USE_MAG_AK8963
        if (ak8963Detect(dev)) {
#ifdef MAG_AK8963_ALIGN
            dev->magAlign.onBoard = MAG_AK8963_ALIGN;
#endif
            magHardware = MAG_AK8963;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_GPS:
#ifdef USE_GPS
        if (gpsMagDetect(dev)) {
#ifdef MAG_GPS_ALIGN
            dev->magAlign.onBoard = MAG_GPS_ALIGN;
#endif
            magHardware = MAG_GPS;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_MAG3110:
#ifdef USE_MAG_MAG3110
        if (mag3110detect(dev)) {
#ifdef MAG_MAG3110_ALIGN
            dev->magAlign.onBoard = MAG_MAG3110_ALIGN;
#endif
            magHardware = MAG_MAG3110;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_IST8310:
#ifdef USE_MAG_IST8310
        if (ist8310Detect(dev)) {
#ifdef MAG_IST8310_ALIGN
            dev->magAlign.onBoard = MAG_IST8310_ALIGN;
#endif
            magHardware = MAG_IST8310;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_IST8308:
#ifdef USE_MAG_IST8308
        if (ist8308Detect(dev)) {
#ifdef MAG_IST8308_ALIGN
            dev->magAlign.onBoard = MAG_IST8308_ALIGN;
#endif
            magHardware = MAG_IST8308;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_MPU9250:
#ifdef USE_MAG_MPU9250
        if (mpu9250CompassDetect(dev)) {
#ifdef MAG_MPU9250_ALIGN
            dev->magAlign.onBoard = MAG_MPU9250_ALIGN;
#endif
            magHardware = MAG_MPU9250;
            break;
        }
#endif
        FALLTHROUGH;

    case MAG_LIS3MDL:
#ifdef USE_MAG_LIS3MDL
        if (lis3mdlDetect(dev)) {
#ifdef MAG_LIS3MDL_ALIGN
            dev->magAlign = MAG_LIS3MDL_ALIGN;
#endif
            magHardware = MAG_LIS3MDL;
            break;
        }
#endif

        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_FAKE:
#ifdef USE_FAKE_MAG
        if (fakeMagDetect(dev)) {
            magHardware = MAG_FAKE;
            break;
        }
#endif
        /* If we are asked for a specific sensor - break out, otherwise - fall through and continue */
        if (magHardwareToUse != MAG_AUTODETECT) {
            break;
        }
        FALLTHROUGH;

    case MAG_NONE:
        magHardware = MAG_NONE;
        break;
    }

    addBootlogEvent6(BOOT_EVENT_MAG_DETECTION, BOOT_EVENT_FLAGS_NONE, magHardware, 0, 0, 0);

    if (magHardware == MAG_NONE) {
        sensorsClear(SENSOR_MAG);
        return false;
    }

    detectedSensors[SENSOR_INDEX_MAG] = magHardware;
    sensorsSet(SENSOR_MAG);
    return true;
}

bool compassInit(void)
{
#ifdef USE_DUAL_MAG
    mag.dev.magSensorToUse = compassConfig()->mag_to_use;
#else
    mag.dev.magSensorToUse = 0;
#endif

    if (!compassDetect(&mag.dev, compassConfig()->mag_hardware)) {
        return false;
    }
    // initialize and calibration. turn on led during mag calibration (calibration routine blinks it)
    LED1_ON;
    const bool ret = mag.dev.init(&mag.dev);
    LED1_OFF;

    if (!ret) {
        addBootlogEvent2(BOOT_EVENT_MAG_INIT_FAILED, BOOT_EVENT_FLAGS_ERROR);
        sensorsClear(SENSOR_MAG);
    }

    if (compassConfig()->rollDeciDegrees != 0 ||
        compassConfig()->pitchDeciDegrees != 0 ||
        compassConfig()->yawDeciDegrees != 0) {

        // Externally aligned compass
        mag.dev.magAlign.useExternal = true;

        fp_angles_t compassAngles = {
             .angles.roll = DECIDEGREES_TO_RADIANS(compassConfig()->rollDeciDegrees),
             .angles.pitch = DECIDEGREES_TO_RADIANS(compassConfig()->pitchDeciDegrees),
             .angles.yaw = DECIDEGREES_TO_RADIANS(compassConfig()->yawDeciDegrees),
        };
        rotationMatrixFromAngles(&mag.dev.magAlign.externalRotation, &compassAngles);
    } else {
        mag.dev.magAlign.useExternal = false;
        if (compassConfig()->mag_align != ALIGN_DEFAULT) {
            mag.dev.magAlign.onBoard = compassConfig()->mag_align;
        }
    }

    return ret;
}

bool compassIsHealthy(void)
{
    return (mag.magADC[X] != 0) || (mag.magADC[Y] != 0) || (mag.magADC[Z] != 0);
}

bool compassIsReady(void)
{
    return magUpdatedAtLeastOnce;
}

void compassUpdate(timeUs_t currentTimeUs)
{
    static sensorCalibrationState_t calState;
    static timeUs_t calStartedAt = 0;
    static int16_t magPrev[XYZ_AXIS_COUNT];

    // Check magZero
    if ((compassConfig()->magZero.raw[X] == 0) && (compassConfig()->magZero.raw[Y] == 0) && (compassConfig()->magZero.raw[Z] == 0)) {
        DISABLE_STATE(COMPASS_CALIBRATED);
    }
    else {
        ENABLE_STATE(COMPASS_CALIBRATED);
    }

    if (!mag.dev.read(&mag.dev)) {
        mag.magADC[X] = 0;
        mag.magADC[Y] = 0;
        mag.magADC[Z] = 0;
        return;
    }

    for (int axis = 0; axis < XYZ_AXIS_COUNT; axis++) {
        mag.magADC[axis] = mag.dev.magADCRaw[axis];  // int32_t copy to work with
    }

    if (STATE(CALIBRATE_MAG)) {
        calStartedAt = currentTimeUs;

        for (int axis = 0; axis < 3; axis++) {
            compassConfigMutable()->magZero.raw[axis] = 0;
            magPrev[axis] = 0;
        }

        beeper(BEEPER_ACTION_SUCCESS);

        sensorCalibrationResetState(&calState);
        DISABLE_STATE(CALIBRATE_MAG);
    }

    if (calStartedAt != 0) {
        if ((currentTimeUs - calStartedAt) < (compassConfig()->magCalibrationTimeLimit * 1000000)) {
            LED0_TOGGLE;

            float diffMag = 0;
            float avgMag = 0;

            for (int axis = 0; axis < 3; axis++) {
                diffMag += (mag.magADC[axis] - magPrev[axis]) * (mag.magADC[axis] - magPrev[axis]);
                avgMag += (mag.magADC[axis] + magPrev[axis]) * (mag.magADC[axis] + magPrev[axis]) / 4.0f;
            }

            // sqrtf(diffMag / avgMag) is a rough approximation of tangent of angle between magADC and magPrev. tan(8 deg) = 0.14
            if ((avgMag > 0.01f) && ((diffMag / avgMag) > (0.14f * 0.14f))) {
                sensorCalibrationPushSampleForOffsetCalculation(&calState, mag.magADC);

                for (int axis = 0; axis < 3; axis++) {
                    magPrev[axis] = mag.magADC[axis];
                }
            }
        } else {
            float magZerof[3];
            sensorCalibrationSolveForOffset(&calState, magZerof);

            for (int axis = 0; axis < 3; axis++) {
                compassConfigMutable()->magZero.raw[axis] = lrintf(magZerof[axis]);
            }

            calStartedAt = 0;
            saveConfigAndNotify();
        }
    }
    else {
        mag.magADC[X] -= compassConfig()->magZero.raw[X];
        mag.magADC[Y] -= compassConfig()->magZero.raw[Y];
        mag.magADC[Z] -= compassConfig()->magZero.raw[Z];
    }

    if (mag.dev.magAlign.useExternal) {
        const fpVector3_t v = {
            .x = mag.magADC[X],
            .y = mag.magADC[Y],
            .z = mag.magADC[Z],
         };

        fpVector3_t rotated;

        rotationMatrixRotateVector(&rotated, &v, &mag.dev.magAlign.externalRotation);

         mag.magADC[X] = rotated.x;
         mag.magADC[Y] = rotated.y;
         mag.magADC[Z] = rotated.z;

    } else {
        // On-board compass
        applySensorAlignment(mag.magADC, mag.magADC, mag.dev.magAlign.onBoard);
        applyBoardAlignment(mag.magADC);
    }

    magUpdatedAtLeastOnce = 1;
}
#endif
