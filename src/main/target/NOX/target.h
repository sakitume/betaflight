/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define USE_TARGET_CONFIG

#define TARGET_BOARD_IDENTIFIER "NOX1"
#define USBD_PRODUCT_STRING "NoxF4V1"

#define LED0_PIN                PA4

#define USE_BEEPER
#define BEEPER_PIN              PC13
#define BEEPER_INVERTED

#define INVERTER_PIN_UART2      PC14

#define USE_ACC
#define USE_ACC_SPI_MPU6500
#define USE_ACC_SPI_MPU6000

#define USE_GYRO
#define USE_GYRO_SPI_MPU6500
#define USE_GYRO_SPI_MPU6000

#define GYRO_1_CS_PIN           PB12
#define GYRO_1_SPI_INSTANCE     SPI2

#define USE_EXTI
#define USE_GYRO_EXTI
#define GYRO_1_EXTI_PIN         PA8
#define USE_MPU_DATA_READY_SIGNAL

#define USE_BARO
#define USE_BARO_BMP280
#define USE_BARO_SPI_BMP280
#define BARO_SPI_INSTANCE       SPI2
#define BARO_CS_PIN             PA9

#define USE_MAX7456
#define MAX7456_SPI_INSTANCE    SPI2
#define MAX7456_SPI_CS_PIN      PA10
#define MAX7456_SPI_CLK         (SPI_CLOCK_STANDARD*2)
#define MAX7456_RESTORE_CLK     (SPI_CLOCK_FAST)

#define USE_VCP
#define USE_USB_DETECT
//#define USB_DETECT_PIN   PC5

#define USE_UART1
#define UART1_RX_PIN            PB7
#define UART1_TX_PIN            PB6

#define USE_UART2
#define UART2_RX_PIN            PA3
#define UART2_TX_PIN            PA2

#define USE_SOFTSERIAL1
#define SOFTSERIAL1_RX_PIN      PA2    // Backdoor timer on UART2_TX, used for ESC telemetry
#define SOFTSERIAL1_TX_PIN      PA2    // Workaround for softserial not initializing with only RX

#define USE_SOFTSERIAL2
#define SOFTSERIAL2_RX_PIN      NONE
#define SOFTSERIAL2_TX_PIN      NONE

#define SERIAL_PORT_COUNT       5 //VCP, USART1, USART2, SOFTSERIAL1, SOFTSERIAL2

#define USE_ESCSERIAL
#define ESCSERIAL_TIMER_TX_PIN  PB10

#define USE_SPI
#define USE_SPI_DEVICE_1
#define SPI1_SCK_PIN            PB3
#define SPI1_MISO_PIN           PB4
#define SPI1_MOSI_PIN           PB5

#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15

#define USE_FLASHFS
#define USE_FLASH_M25P16
#define FLASH_SPI_INSTANCE      SPI1
#define FLASH_CS_PIN            PA15
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT

#define USE_ADC
#define CURRENT_METER_ADC_PIN   NONE // PA6 Available from TP33
#define VBAT_ADC_PIN            PA5  // 11:1 (10K + 1K) divider

#define DEFAULT_VOLTAGE_METER_SOURCE VOLTAGE_METER_ESC
#define DEFAULT_CURRENT_METER_SOURCE CURRENT_METER_ESC

#define SERIALRX_UART           SERIAL_PORT_USART2

#define USE_TRANSPONDER

#define DEFAULT_RX_FEATURE      FEATURE_RX_SERIAL
#define SERIALRX_PROVIDER       SERIALRX_SBUS

#define DEFAULT_FEATURES        (FEATURE_OSD | FEATURE_SOFTSERIAL | FEATURE_ESC_SENSOR)

#define TARGET_IO_PORTA (0xffff & ~(BIT(14)|BIT(13)))
#define TARGET_IO_PORTB (0xffff & ~(BIT(2)|BIT(11)))
#define TARGET_IO_PORTC (BIT(13)|BIT(14)|BIT(15))

#define USABLE_TIMER_CHANNEL_COUNT 8
#define USED_TIMERS ( TIM_N(1) | TIM_N(2) | TIM_N(3) | TIM_N(4) | TIM_N(5) | TIM_N(9) )


//------------------------------------------------------------------------------
// Resource changes needed by this particular target for software spi nrf24 rx
//------------------------------------------------------------------------------

#ifdef SERIALRX_UART
#undef SERIALRX_UART
#endif

#ifdef DEFAULT_RX_FEATURE
#undef DEFAULT_RX_FEATURE
#endif

#ifdef SERIALRX_PROVIDER
#undef SERIALRX_PROVIDER
#endif

// Nordic Semiconductor uses 'CSN', STM uses 'NSS'
// Here is where you specify your software SPI pins
// The "RX_NSS_PIN" is for the CS/CSN/SS (chip select, chip select not, slave select) pin
#undef RX_SPI_EXTI_PIN	// not using this pin
#undef RX_NSS_PIN

//#define RX_NSS_PIN              PB7		// PB7 is the "RX1" pad on a Play F4 FC board
#define RX_NSS_PIN              PA2		// PA2 is the "TX2" pad on a Play F4 FC board

#define RX_SCK_PIN              PA0
#define RX_MISO_PIN             PB10
#define RX_MOSI_PIN             PB6

// I am not using the CE pin on the NRF24L01. Instead I am tying it to VCC. This works fine
// and just means the module will always be in either RX or TX mode (versus being off).
// If you wish to use the CE pin (like the original rx_nrf24l01.c source code expects)
// then you just need to define the following with the pin you wish to use (replace the P??)
//#define RX_CE_PIN		P??


//--------------------------------------------------------------------------------------------------
// Changes/Additions needed for software spi nr42l01 RX support
//--------------------------------------------------------------------------------------------------
// Define USE_RX_SPI so src/main/rx_spi.c is compiled
#define USE_RX_SPI

// Define USE_RX_SOFTSPI so that bus_spi.c doesn't try to: IO_TAG(RX_NSS_PIN)
// and that src/main/drivers/rx/rx_spi.c is *NOT* compiled, and instead my
// new src/main/drivers/rx/rx_softspi.c is compiled instead
// Note: I had to edit src/main/drivers/rx/rx_spi.c so it would test for this
#define USE_RX_SOFTSPI

// Define USE_RX_NRF24 so that nrf24l01.c is compiled
#define USE_RX_NRF24

// I'm using software SPI so don't define RX_SPI_INSTANCE
#ifdef RX_SPI_INSTANCE
#undef RX_SPI_INSTANCE
#endif
#define RX_SPI_INSTANCE         NULL

// These are the available nrf24l01 spi rx modules I want to try out
#define USE_RX_CX10
#define USE_RX_H8_3D
//#define USE_RX_INAV I didn't see how to enable this with multiprotocol module so am leaving this out
//#define USE_RX_KN This is getting a compile error, looks fixable but too much trouble right now
#define USE_RX_SYMA
#define USE_RX_V202

// Set a default rx protocol module
#undef RX_SPI_DEFAULT_PROTOCOL
#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_NRF24_V202_1M
//#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_NRF24_INAV
//#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_NRF24_H8_3D
//#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_NRF24_CX10A
//#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_NRF24_V202_1M

#ifdef DEFAULT_RX_FEATURE
#undef DEFAULT_RX_FEATURE
#endif
#define DEFAULT_RX_FEATURE      FEATURE_RX_SPI
//#define USE_TELEMETRY
//#define USE_TELEMETRY_LTM
//#define USE_TELEMETRY_NRF24_LTM

// We're using USE_RX_SPI instead
#ifdef USE_PWM
#undef USE_PWM
#endif

// We're using USE_RX_SPI instead
#ifdef USE_PPM
#undef USE_PPM
#endif

// We're using USE_RX_SPI instead
#ifdef USE_SERIAL_RX
#undef USE_SERIAL_RX
#endif

#undef USE_SERIAL_RX
#undef USE_SERIALRX_CRSF       // Team Black Sheep Crossfire protocol
#undef USE_SERIALRX_IBUS       // FlySky and Turnigy receivers
#undef USE_SERIALRX_SBUS       // Frsky and Futaba receivers
#undef USE_SERIALRX_SPEKTRUM   // SRXL, DSM2 and DSMX protocol
#undef USE_SERIALRX_SUMD       // Graupner Hott protocol
#undef USE_SERIALRX_SUMH       // Graupner legacy protocol
#undef USE_SERIALRX_XBUS       // JR

//--------------------------------------------------------------------------------------------------
// Changes/Additions you must make to the corresponding target.mk for software spi nr42l01 RX support
// Note: Make sure tab characters (for whitespace) are used when editing the target.mk
//--------------------------------------------------------------------------------------------------
#if 0
			drivers/rx/rx_nrf24l01.c \
			drivers/rx/rx_softspi.c \
			rx/nrf24_cx10.c \
			rx/nrf24_h8_3d.c \
			rx/nrf24_syma.c \
			rx/nrf24_v202.c
#endif
