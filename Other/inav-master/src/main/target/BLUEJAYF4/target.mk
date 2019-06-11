F405_TARGETS    += $(TARGET)
FEATURES        += SDCARD VCP ONBOARDFLASH

TARGET_SRC = \
            drivers/accgyro/accgyro_mpu6500.c \
            drivers/accgyro/accgyro_mpu9250.c \
            drivers/barometer/barometer_bmp085.c \
            drivers/barometer/barometer_bmp280.c \
            drivers/barometer/barometer_ms56xx.c \
            drivers/compass/compass_hmc5883l.c \
            drivers/compass/compass_qmc5883l.c \
            drivers/compass/compass_ist8310.c \
            drivers/compass/compass_ist8308.c \
            drivers/compass/compass_mag3110.c \
            drivers/compass/compass_lis3mdl.c \
            drivers/light_ws2811strip.c