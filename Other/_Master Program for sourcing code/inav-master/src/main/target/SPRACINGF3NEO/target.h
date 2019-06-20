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

#define TARGET_BOARD_IDENTIFIER "SP3N"

#define LED0                    PB9
#define LED1                    PB2

#define BEEPER                  PC15
#define BEEPER_INVERTED

#define USE_EXTI
#define GYRO_INT_EXTI            PC13
#define USE_MPU_DATA_READY_SIGNAL
#define ENSURE_MPU_DATA_READY_IS_LOW

#define USE_GYRO
#define USE_GYRO_MPU6500
#define USE_GYRO_MPU9250

#define USE_ACC
#define USE_ACC_MPU6500
#define USE_ACC_MPU9250

#define ACC_MPU6500_ALIGN       CW0_DEG
#define GYRO_MPU6500_ALIGN      CW0_DEG
#define ACC_MPU9250_ALIGN       CW0_DEG
#define GYRO_MPU9250_ALIGN      CW0_DEG

#define MPU6500_CS_PIN          SPI1_NSS_PIN
#define MPU6500_SPI_BUS         BUS_SPI1

#define MPU9250_CS_PIN          SPI1_NSS_PIN
#define MPU9250_SPI_BUS         BUS_SPI1

#define USE_BARO
#define BARO_I2C_BUS            BUS_I2C1
#define USE_BARO_BMP280
#define USE_BARO_MS5611

#define USE_MAG
#define MAG_I2C_BUS             BUS_I2C1
#define USE_MAG_MPU9250
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_IST8310
#define USE_MAG_IST8308
#define USE_MAG_MAG3110
#define USE_MAG_LIS3MDL

#define USE_VCP
#define USE_UART1
#define USE_UART2
#define USE_UART3
#define USE_UART4
#define USE_UART5
//#define USE_SOFTSERIAL1
//#define USE_SOFTSERIAL2
//#define SERIAL_PORT_COUNT       8
#define SERIAL_PORT_COUNT       6

#define UART1_TX_PIN            PA9
#define UART1_RX_PIN            PA10

#define UART2_TX_PIN            PA2
#define UART2_RX_PIN            PA3

#define UART3_TX_PIN            PB10
#define UART3_RX_PIN            PB11

#define USE_I2C
#define USE_I2C_DEVICE_1

#define USE_SPI
#define USE_SPI_DEVICE_1 // MPU
#define USE_SPI_DEVICE_2 // SDCard
#define USE_SPI_DEVICE_3 // External (MAX7456 & RTC6705)

#define SPI1_NSS_PIN            PA4
#define SPI1_SCK_PIN            PA5
#define SPI1_MISO_PIN           PA6
#define SPI1_MOSI_PIN           PA7

#define SPI2_NSS_PIN            PB12
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15
#define SPI2_CLOCK_LEADING_EDGE

#define SPI3_NSS_PIN            PA15
#define SPI3_SCK_PIN            PB3
#define SPI3_MISO_PIN           PB4
#define SPI3_MOSI_PIN           PB5

#define USE_VTX_RTC6705
#define VTX_RTC6705_OPTIONAL    // VTX/OSD board is OPTIONAL

#undef USE_VTX_FFPV
#undef USE_VTX_SMARTAUDIO           // Disabled due to flash size
#undef USE_VTX_TRAMP                // Disabled due to flash size
#undef USE_PWM_SERVO_DRIVER         // Disabled due to RAM size

#undef USE_PITOT                    // Disabled due to RAM size
#undef USE_PITOT_ADC                // Disabled due to RAM size

#define RTC6705_CS_PIN          PF4
#define RTC6705_SPI_INSTANCE    SPI3
#define RTC6705_POWER_PIN       PC3

#define USE_RTC6705_CLK_HACK
#define RTC6705_CLK_PIN         SPI3_SCK_PIN

#define USE_MAX7456
#define MAX7456_SPI_BUS         BUS_SPI3
#define MAX7456_CS_PIN          PA15

#define SPI_SHARED_MAX7456_AND_RTC6705

#define USE_SDCARD
#define USE_SDCARD_SPI
#define SDCARD_DETECT_INVERTED
#define SDCARD_DETECT_PIN       PC14
#define SDCARD_SPI_BUS          BUS_SPI2
#define SDCARD_CS_PIN           SPI2_NSS_PIN

#define USE_ADC
#define ADC_INSTANCE            ADC1
#define ADC_CHANNEL_1_PIN               PC1
#define ADC_CHANNEL_2_PIN               PC2
#define ADC_CHANNEL_3_PIN               PC0
#define VBAT_ADC_CHANNEL                ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL       ADC_CHN_2
#define RSSI_ADC_CHANNEL                ADC_CHN_3

#define USE_LED_STRIP
#define WS2811_PIN                      PA8

#define ENABLE_BLACKBOX_LOGGING_ON_SDCARD_BY_DEFAULT

#define USE_OSD

#define DEFAULT_RX_TYPE         RX_TYPE_SERIAL
#define DEFAULT_FEATURES        (FEATURE_TX_PROF_SEL | FEATURE_TRANSPONDER | FEATURE_RSSI_ADC | FEATURE_TELEMETRY | FEATURE_OSD | FEATURE_LED_STRIP)
#define SERIALRX_UART           SERIAL_PORT_USART2
#define GPS_UART                SERIAL_PORT_USART3
#define TELEMETRY_UART          SERIAL_PORT_USART5
#define SERIALRX_PROVIDER       SERIALRX_SBUS

#define BUTTONS
#define BUTTON_A_PIN            PD2

#define SPEKTRUM_BIND_PIN       UART2_RX_PIN

#define BINDPLUG_PIN            PD2

#define USE_SERIAL_4WAY_BLHELI_INTERFACE

// Number of available PWM outputs
#define MAX_PWM_OUTPUT_PORTS    10

// IO - assuming 303 in 64pin package, TODO
#define TARGET_IO_PORTA 0xffff
#define TARGET_IO_PORTB 0xffff
#define TARGET_IO_PORTC 0xffff
#define TARGET_IO_PORTD (BIT(2))
#define TARGET_IO_PORTF (BIT(0)|BIT(1)|BIT(4))
