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

#define TARGET_BOARD_IDENTIFIER "MF4S"
#define USBD_PRODUCT_STRING  "Matek_F405SE"

#define LED0                    PA14  //Blue
#define LED1                    PA13  //Green
#define BEEPER                  PC15
#define BEEPER_INVERTED

// *************** SPI1 Gyro & ACC *******************
#define USE_SPI
#define USE_SPI_DEVICE_1

#define SPI1_SCK_PIN            PA5
#define SPI1_MISO_PIN   	    PA6
#define SPI1_MOSI_PIN   	    PA7

#define MPU6000_CS_PIN          PA4
#define MPU6000_SPI_BUS         BUS_SPI1

#define USE_EXTI
#define GYRO_INT_EXTI            PC4
#define USE_MPU_DATA_READY_SIGNAL

#define USE_GYRO
#define USE_GYRO_MPU6000
#define GYRO_MPU6000_ALIGN      CW270_DEG

#define USE_ACC
#define USE_ACC_MPU6000
#define ACC_MPU6000_ALIGN       CW270_DEG

// *************** I2C /Baro/Mag *********************
#define USE_I2C
#define USE_I2C_DEVICE_1
#define I2C1_SCL                PB8
#define I2C1_SDA                PB9

#define USE_I2C_DEVICE_2
#define I2C2_SCL                PB10
#define I2C2_SDA                PB11

#define USE_BARO
#define BARO_I2C_BUS            BUS_I2C1
#define USE_BARO_BMP280
#define USE_BARO_MS5611
#define USE_BARO_BMP085

#define USE_MAG
#define MAG_I2C_BUS             BUS_I2C2
#define USE_MAG_AK8963
#define USE_MAG_AK8975
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_IST8310
#define USE_MAG_MAG3110
#define USE_MAG_LIS3MDL

#define USE_RANGEFINDER
#define USE_RANGEFINDER_HCSR04_I2C
#define RANGEFINDER_I2C_BUS     BUS_I2C2

#define PITOT_I2C_BUS           BUS_I2C2

#define TEMPERATURE_I2C_BUS     BUS_I2C2


// *************** SPI2 OSD ***************************
#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN   	    PC2
#define SPI2_MOSI_PIN   	    PC3

#define USE_OSD
#define USE_MAX7456
#define MAX7456_CS_PIN          PB12
#define MAX7456_SPI_BUS         BUS_SPI2

// *************** SPI3 SD Card  ********************
#define USE_SDCARD
#define USE_SDCARD_SPI
#define SDCARD_SPI_BUS          BUS_SPI3
#define SDCARD_CS_PIN           PC14

#define USE_SPI_DEVICE_3
#define SPI3_SCK_PIN            PB3
#define SPI3_MISO_PIN   	    PB4
#define SPI3_MOSI_PIN   	    PB5
#define SPI3_CLOCK_LEADING_EDGE

#define ENABLE_BLACKBOX_LOGGING_ON_SDCARD_BY_DEFAULT

// *************** UART *****************************
#define USE_VCP
#define VBUS_SENSING_PIN        PC13
#define VBUS_SENSING_ENABLED

#define USE_UART1
#define UART1_TX_PIN            PA9
#define UART1_RX_PIN            PA10

#define USE_UART2
#define UART2_TX_PIN            PA2
#define UART2_RX_PIN            PA3

#define USE_UART3
#define UART3_TX_PIN            PC10
#define UART3_RX_PIN            PC11

#define USE_UART4
#define UART4_TX_PIN            PA0
#define UART4_RX_PIN            PA1

#define USE_UART5
#define UART5_TX_PIN            PC12
#define UART5_RX_PIN            PD2

#define USE_UART6
#define UART6_TX_PIN            PC6
#define UART6_RX_PIN            PC7

#define USE_SOFTSERIAL1               //Frsky SmartPort on TX2 pad
#define SOFTSERIAL_1_TX_PIN      PA2
#define SOFTSERIAL_1_RX_PIN      PA2

#define SERIAL_PORT_COUNT       8

#define DEFAULT_RX_TYPE         RX_TYPE_SERIAL
#define SERIALRX_PROVIDER       SERIALRX_SBUS
#define SERIALRX_UART           SERIAL_PORT_USART2

// *************** ADC ***************************
#define USE_ADC
#define ADC_INSTANCE                ADC1
#define ADC1_DMA_STREAM             DMA2_Stream0
#define ADC_CHANNEL_1_PIN           PC0
#define ADC_CHANNEL_2_PIN           PC1
#define ADC_CHANNEL_3_PIN           PC5
#define VBAT_ADC_CHANNEL            ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL   ADC_CHN_2
#define RSSI_ADC_CHANNEL            ADC_CHN_3

// *************** LED2812 ************************
#define USE_LED_STRIP
#define WS2811_PIN                      PA15
#define WS2811_DMA_HANDLER_IDENTIFER    DMA1_ST5_HANDLER
#define WS2811_DMA_STREAM               DMA1_Stream5
#define WS2811_DMA_CHANNEL              DMA_Channel_3

// ***************  OTHERS *************************
#define DEFAULT_FEATURES        (FEATURE_TX_PROF_SEL | FEATURE_OSD | FEATURE_CURRENT_METER | FEATURE_VBAT | FEATURE_TELEMETRY | FEATURE_SOFTSERIAL)
#define CURRENT_METER_SCALE   317

#define USE_SPEKTRUM_BIND
#define BIND_PIN                PA3 //  RX2

#define USE_SERIAL_4WAY_BLHELI_INTERFACE

#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff
#define TARGET_IO_PORTD         (BIT(2))

#define MAX_PWM_OUTPUT_PORTS       9

#define PCA9685_I2C_BUS         BUS_I2C2
