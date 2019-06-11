
/**
 * Collects hardware failure information from devices.
 * No hardware checking should be actually performed,
 * rather already known hardware status should be returned.
 */

typedef enum {
    HW_SENSOR_NONE          = 0,    // Not selected
    HW_SENSOR_OK            = 1,    // Selected, detected and healthy (ready to be used)
    HW_SENSOR_UNAVAILABLE   = 2,    // Selected in configuration but not detected
    HW_SENSOR_UNHEALTHY     = 3,    // Selected, detected but is reported as not healthy
} hardwareSensorStatus_e;

// Sensor is considered healthy if it's either
// not configured or either configured and working.
#define HW_SENSOR_IS_HEALTHY(status)    (status == HW_SENSOR_NONE || status == HW_SENSOR_OK)

hardwareSensorStatus_e getHwGyroStatus(void);
hardwareSensorStatus_e getHwAccelerometerStatus(void);
hardwareSensorStatus_e getHwCompassStatus(void);
hardwareSensorStatus_e getHwBarometerStatus(void);
hardwareSensorStatus_e getHwGPSStatus(void);
hardwareSensorStatus_e getHwRangefinderStatus(void);
hardwareSensorStatus_e getHwPitotmeterStatus(void);
hardwareSensorStatus_e getHwOpticalFlowStatus(void);

bool isHardwareHealthy(void);

