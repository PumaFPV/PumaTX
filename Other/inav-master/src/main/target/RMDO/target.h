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

#define TARGET_BOARD_IDENTIFIER "RMDO" // Ready Made RC DoDo

#define LED0                    PB3

#define BEEPER                  PC15
#define BEEPER_INVERTED

#define USE_EXTI
#define USE_MPU_DATA_READY_SIGNAL
#define ENSURE_MPU_DATA_READY_IS_LOW

#define USE_GYRO
#define USE_ACC

#define USE_GYRO_MPU6050
#define USE_ACC_MPU6050
#define MPU6050_I2C_BUS         BUS_I2C1

#define GYRO_MPU6050_ALIGN      CW270_DEG
#define ACC_MPU6050_ALIGN       CW270_DEG

#define USE_BARO
#define BARO_I2C_BUS            BUS_I2C1
#define USE_BARO_BMP280

#define USE_MAG
#define MAG_I2C_BUS             BUS_I2C1
#define USE_MAG_AK8975
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_IST8310
#define USE_MAG_IST8308
#define USE_MAG_MAG3110
#define USE_MAG_LIS3MDL

#define USE_FLASHFS
#define USE_FLASH_M25P16

#define USE_RANGEFINDER
#define USE_RANGEFINDER_HCSR04
#define RANGEFINDER_HCSR04_TRIGGER_PIN       PB0 // RC_CH7 (PB0) - only 3.3v ( add a 1K Ohms resistor )
#define RANGEFINDER_HCSR04_ECHO_PIN          PB1 // RC_CH8 (PB1) - only 3.3v ( add a 1K Ohms resistor )

#define USE_UART1
#define USE_UART2
#define USE_UART3
#define USE_SOFTSERIAL1
#define USE_SOFTSERIAL2
#define SERIAL_PORT_COUNT       5

#define UART1_TX_PIN            PA9
#define UART1_RX_PIN            PA10

#define UART2_TX_PIN            PA14 // PA14 / SWCLK
#define UART2_RX_PIN            PA15 // PA15

#define UART3_TX_PIN            PB10 // PB10 (AF7)
#define UART3_RX_PIN            PB11 // PB11 (AF7)

#define SOFTSERIAL_1_RX_PIN     PB4
#define SOFTSERIAL_1_TX_PIN     PB5
#define SOFTSERIAL_2_RX_PIN     PB0
#define SOFTSERIAL_2_TX_PIN     PB1

#define USE_I2C
#define USE_I2C_DEVICE_1

#define USE_SPI
#define USE_SPI_DEVICE_2 // PB12,13,14,15 on AF5

#define M25P16_CS_GPIO          GPIOB
#define M25P16_CS_PIN           PB12
#define M25P16_SPI_BUS          BUS_SPI2

#define BOARD_HAS_VOLTAGE_DIVIDER
#define USE_ADC
#define ADC_INSTANCE            ADC2
#define ADC_CHANNEL_1_PIN               PA4
#define ADC_CHANNEL_2_PIN               PA5
#define ADC_CHANNEL_3_PIN               PB2
#define VBAT_ADC_CHANNEL                ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL       ADC_CHN_2
#define RSSI_ADC_CHANNEL                ADC_CHN_3

#define USE_LED_STRIP

#define USE_LED_STRIP_ON_DMA1_CHANNEL2
#define WS2811_PIN                      PA8

#undef NAV_MAX_WAYPOINTS
#define NAV_MAX_WAYPOINTS       30

#define USE_SPEKTRUM_BIND
// USART3,
#define BIND_PIN                PB11

#define USE_SERIAL_4WAY_BLHELI_INTERFACE

#undef USE_GPS_PROTO_NAZA
#undef USE_TELEMETRY_HOTT
#undef USE_TELEMETRY_SMARTPORT
#undef USE_TELEMETRY_IBUS

// Number of available PWM outputs
#define MAX_PWM_OUTPUT_PORTS    12

// IO - stm32f303cc in 48pin package
#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         (BIT(13)|BIT(14)|BIT(15))
#define TARGET_IO_PORTF         (BIT(0)|BIT(1)|BIT(4))
