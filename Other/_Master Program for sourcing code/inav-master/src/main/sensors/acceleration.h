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

#include "common/axis.h"
#include "common/maths.h"
#include "common/vector.h"
#include "config/parameter_group.h"
#include "drivers/accgyro/accgyro.h"
#include "sensors/sensors.h"

#define GRAVITY_CMSS    980.665f
#define GRAVITY_MSS     9.80665f

#define ACC_CLIPPING_THRESHOLD_G        7.9f
#define ACC_VIBE_FLOOR_FILT_HZ          5.0f
#define ACC_VIBE_FILT_HZ                2.0f

// Type of accelerometer used/detected
typedef enum {
    ACC_NONE = 0,
    ACC_AUTODETECT = 1,
    ACC_ADXL345 = 2,
    ACC_MPU6050 = 3,
    ACC_MMA8452 = 4,
    ACC_BMA280 = 5,
    ACC_LSM303DLHC = 6,
    ACC_MPU6000 = 7,
    ACC_MPU6500 = 8,
    ACC_MPU9250 = 9,
    ACC_BMI160 = 10,
    ACC_ICM20689 = 11,
    ACC_FAKE = 12,
    ACC_MAX = ACC_FAKE
} accelerationSensor_e;

typedef struct acc_s {
    accDev_t dev;
    uint32_t accTargetLooptime;
    float accADCf[XYZ_AXIS_COUNT]; // acceleration in g
    float accVibeSq[XYZ_AXIS_COUNT];
    uint32_t accClipCount;
} acc_t;

extern acc_t acc;

typedef struct accelerometerConfig_s {
    sensor_align_e acc_align;               // acc alignment
    uint8_t acc_hardware;                   // Which acc hardware to use on boards with more than one device
    uint16_t acc_lpf_hz;                    // cutoff frequency for the low pass filter used on the acc z-axis for althold in Hz
    flightDynamicsTrims_t accZero;          // Accelerometer offset
    flightDynamicsTrims_t accGain;          // Accelerometer gain to read exactly 1G
    uint8_t acc_notch_hz;                   // Accelerometer notch filter frequency
    uint8_t acc_notch_cutoff;               // Accelerometer notch filter cutoff frequency
} accelerometerConfig_t;

PG_DECLARE(accelerometerConfig_t, accelerometerConfig);

bool accInit(uint32_t accTargetLooptime);
bool accIsCalibrationComplete(void);
void accStartCalibration(void);
void accGetMeasuredAcceleration(fpVector3_t *measuredAcc);
void accGetVibrationLevels(fpVector3_t *accVibeLevels);
float accGetVibrationLevel(void);
uint32_t accGetClipCount(void);
void accUpdate(void);
void accSetCalibrationValues(void);
void accInitFilters(void);
bool accIsHealthy(void);
bool accGetCalibrationAxisStatus(int axis);
uint8_t accGetCalibrationAxisFlags(void);
