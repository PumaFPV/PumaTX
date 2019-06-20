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

#define TARGET_BOARD_IDENTIFIER "KFCi"
#define USE_HARDWARE_PREBOOT_SETUP

#define LED0                    PC13
#define LED0_INVERTED

#define LED1                    PC14
#define LED1_INVERTED

#define BEEPER                  PA13
#define BEEPER_INVERTED

#define USE_EXTI
#define GYRO_INT_EXTI            PC5
#define USE_MPU_DATA_READY_SIGNAL
#define ENSURE_MPU_DATA_READY_IS_LOW

#define BUS_SPI_SPEED_MAX BUS_SPEED_SLOW

#define MPU6000_CS_PIN          PB5
#define MPU6000_SPI_BUS         BUS_SPI2

#define USE_GYRO
#define USE_GYRO_MPU6000
#define GYRO_MPU6000_ALIGN CW90_DEG

#define USE_ACC
#define USE_ACC_MPU6000
#define ACC_MPU6000_ALIGN CW90_DEG

#define USE_MAG
#define MAG_I2C_BUS             BUS_I2C1
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_IST8310
#define USE_MAG_IST8308
#define USE_MAG_MAG3110
#define USE_MAG_LIS3MDL

#define USE_SPI
#define USE_SPI_DEVICE_2 // PB12,13,14,15 on AF5

#define SPI2_NSS_PIN            PB12
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15

#define USE_FLASHFS

#define USE_FLASH_M25P16
#define M25P16_CS_PIN           PB12
#define M25P16_SPI_BUS          BUS_SPI2
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT

#define USE_BARO
#define USE_BARO_MS5611
#define MS5611_SPI_BUS          BUS_SPI2
#define MS5611_CS_PIN           PB2

#define USE_OSD
#define USE_MAX7456
#define MAX7456_SPI_BUS         BUS_SPI2
#define MAX7456_CS_PIN          PA7

//#define RFM_SPI             SPI2
//#define RFM_SPI_CS_PIN      PC15
//#define RFM_IRQ_PIN         PB3
//#define RFM_SPI_CLK         (SPI_CLOCK_STANDARD*2)
//#define RFM_RESTORE_CLK     (SPI_CLOCK_FAST)

#define USE_RX_SPI
#define USE_RX_ELERES
#define RX_NSS_PIN                  PC15
#define RX_NSS_GPIO_CLK_PERIPHERAL  RCC_APB2Periph_GPIOC
#define RX_SCK_PIN                  PB13
#define RX_MOSI_PIN                 PB15
#define RX_MISO_PIN                 PB14
#define RX_SPI_INSTANCE             SPI2
#define RX_IRQ_PIN                  PB3

#define USE_VCP
#define USE_UART1
#define USE_UART2
#define SERIAL_PORT_COUNT 3

#define UART1_TX_PIN            PA9
#define UART1_RX_PIN            PA10

#define UART2_TX_PIN            PA2
#define UART2_RX_PIN            PA3

#define USE_I2C
#define USE_I2C_DEVICE_1

#define I2C1_SCL                PA15
#define I2C1_SDA                PA14

#define USE_PITOT_MS4525
#define PITOT_I2C_BUS           BUS_I2C1

#define BOARD_HAS_VOLTAGE_DIVIDER
#define USE_ADC
#define ADC_INSTANCE                    ADC2
#define ADC_CHANNEL_1_PIN               PA4
#define ADC_CHANNEL_2_PIN               PA6
#define ADC_CHANNEL_3_PIN               PA5
#define VBAT_ADC_CHANNEL                ADC_CHN_1
#define RSSI_ADC_CHANNEL                ADC_CHN_2
#define CURRENT_METER_ADC_CHANNEL       ADC_CHN_3

#define USE_LED_STRIP
#define WS2811_PIN                      PA8
#define WS2811_DMA_STREAM               DMA1_Channel2
#define WS2811_DMA_HANDLER_IDENTIFER    DMA1_CH2_HANDLER

#define DEFAULT_FEATURES        (FEATURE_TX_PROF_SEL | FEATURE_BLACKBOX | FEATURE_OSD | FEATURE_VBAT)
#define DEFAULT_RX_TYPE         RX_TYPE_PPM

#define USE_SERIAL_4WAY_BLHELI_INTERFACE

// Number of available PWM outputs
#define MAX_PWM_OUTPUT_PORTS    10
#define TARGET_MOTOR_COUNT      10

// IO - stm32f303cc in 48pin package
#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff
#define TARGET_IO_PORTD         0xffff
#define TARGET_IO_PORTF         (BIT(4))
