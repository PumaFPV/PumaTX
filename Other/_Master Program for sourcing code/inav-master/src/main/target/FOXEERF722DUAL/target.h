/*
 * This file is part of INAV.
 *
 * INAV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * INAV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with INAV.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define TARGET_BOARD_IDENTIFIER "FXF7"
#define USBD_PRODUCT_STRING     "FOXEER722DUAL"

/*** Indicators ***/
#define LED0                    PC15
#define BEEPER                  PA4
#define BEEPER_INVERTED

/*** IMU sensors ***/
#define USE_EXTI
#define USE_ACC
#define USE_GYRO

// We use dual IMU sensors, they have to be described in the target file
#define USE_TARGET_IMU_HARDWARE_DESCRIPTORS
#define USE_MPU_DATA_READY_SIGNAL
#define USE_DUAL_GYRO

// MPU6000
#define USE_GYRO_MPU6000
#define USE_ACC_MPU6000

#define MPU6000_CS_PIN          PB2
#define MPU6000_SPI_BUS         BUS_SPI1
#define MPU6000_EXTI_PIN        PC4
#define GYRO_MPU6000_ALIGN      CW270_DEG
#define ACC_MPU6000_ALIGN       CW270_DEG

// ICM20602 - handled by MPU6500 driver
#define USE_GYRO_MPU6500
#define USE_ACC_MPU6500

#define MPU6500_CS_PIN          PB1
#define MPU6500_SPI_BUS         BUS_SPI1
#define MPU6500_EXTI_PIN        PB0
#define GYRO_MPU6500_ALIGN      CW180_DEG
#define ACC_MPU6500_ALIGN       CW180_DEG

/*** SPI/I2C bus ***/
#define USE_SPI
#define USE_SPI_DEVICE_1
#define SPI1_SCK_PIN            PA5
#define SPI1_MISO_PIN           PA6
#define SPI1_MOSI_PIN           PA7

#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15

#define USE_SPI_DEVICE_3
#define SPI3_SCK_PIN            PC10
#define SPI3_MISO_PIN           PC11 
#define SPI3_MOSI_PIN           PB5

#define USE_I2C
#define USE_I2C_DEVICE_1
#define I2C1_SCL                PB8      
#define I2C1_SDA                PB9 

/*** Onboard flash ***/
#define USE_FLASHFS
#define USE_FLASH_M25P16
#define M25P16_CS_PIN           PB12
#define M25P16_SPI_BUS          BUS_SPI2

/*** OSD ***/
#define USE_OSD
#define USE_MAX7456
#define MAX7456_SPI_BUS         BUS_SPI3
#define MAX7456_CS_PIN          PC3

/*** Serial ports ***/
#define USE_VCP

#define USE_UART1
#define UART1_TX_PIN            PB6
#define UART1_RX_PIN            PB7

#define USE_UART2
#define UART2_TX_PIN            PA2
#define UART2_RX_PIN            PA3

#define USE_UART3
#define UART3_TX_PIN            PB10
#define UART3_RX_PIN            PB11

#define USE_UART4
#define UART4_TX_PIN            PA0
#define UART4_RX_PIN            PA1

#define USE_UART5   
#define UART5_TX_PIN            PC12  
#define UART5_RX_PIN            PD2  

#define SERIAL_PORT_COUNT       6

/*** BARO & MAG ***/
#define USE_BARO
#define BARO_I2C_BUS            BUS_I2C1
#define USE_BARO_BMP280
#define USE_BARO_MS5611

#define USE_MAG
#define MAG_I2C_BUS             BUS_I2C2
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_IST8310
#define USE_MAG_MAG3110
#define USE_MAG_LIS3MDL

/*** ADC ***/
#define USE_ADC
#define ADC_CHANNEL_1_PIN               PC0
#define ADC_CHANNEL_2_PIN               PC2
#define ADC_CHANNEL_3_PIN               PA0

#define VBAT_ADC_CHANNEL                ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL       ADC_CHN_2
#define RSSI_ADC_CHANNEL                ADC_CHN_3

/*** LED STRIP ***/
#define USE_LED_STRIP
#define WS2811_PIN                      PA15
 
/*** Default settings ***/
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT
#define SERIALRX_UART                   SERIAL_PORT_USART1
#define DEFAULT_RX_TYPE                 RX_TYPE_SERIAL
#define SERIALRX_PROVIDER               SERIALRX_SBUS
//#define CURRENT_METER_SCALE             166

/*** Timer/PWM output ***/
#define USE_SERIAL_4WAY_BLHELI_INTERFACE
#define MAX_PWM_OUTPUT_PORTS            6
#define USE_DSHOT

/*** Used pins ***/
#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff
#define TARGET_IO_PORTD         (BIT(2))