F405_TARGETS    += $(TARGET)
FEATURES        += VCP ONBOARDFLASH

TARGET_SRC = \
            drivers/accgyro/accgyro_mpu6000.c \
            drivers/accgyro/accgyro_mpu6500.c \
            drivers/max7456.c \
            drivers/light_ws2811strip.c
            