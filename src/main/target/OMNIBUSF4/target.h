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

#if defined(OMNIBUSF4SD)
#define TARGET_BOARD_IDENTIFIER "OBSD"
#elif defined(LUXF4OSD)
#define TARGET_BOARD_IDENTIFIER "LUX4"
#elif defined(DYSF4PRO)
#define TARGET_BOARD_IDENTIFIER "DYS4"
#elif defined(XRACERF4)
#define TARGET_BOARD_IDENTIFIER "XRF4"
#elif defined(EXUAVF4PRO)
#define TARGET_BOARD_IDENTIFIER "EXF4"
#elif defined(SYNERGYF4)
#define TARGET_BOARD_IDENTIFIER "SYN4"
#define TARGET_MANUFACTURER_IDENTIFIER "KLEE"
#else
#define TARGET_BOARD_IDENTIFIER "OBF4"
// Example of a manufacturer ID to be persisted as part of the config:
#define TARGET_MANUFACTURER_IDENTIFIER "AIRB"
#define OMNIBUSF4BASE // For config.c
#endif

#if defined(LUXF4OSD)
#define USBD_PRODUCT_STRING "LuxF4osd"
#elif defined(DYSF4PRO)
#define USBD_PRODUCT_STRING "DysF4Pro"
#elif defined(XRACERF4)
#define USBD_PRODUCT_STRING "XRACERF4"
#elif defined(EXUAVF4PRO)
#define USBD_PRODUCT_STRING "ExuavF4Pro"
#elif defined(SYNERGYF4)
#define USBD_PRODUCT_STRING "SynergyF4"
#else
#define USBD_PRODUCT_STRING "OmnibusF4"
#endif

#define LED0_PIN                PB5
#define USE_BEEPER
#define BEEPER_PIN              PB4
#define BEEPER_INVERTED

#if defined(OMNIBUSF4SD) || defined(DYSF4PRO)
#define ENABLE_DSHOT_DMAR       DSHOT_DMAR_ON
#endif

#ifdef OMNIBUSF4SD
// These inverter control pins collide with timer channels on CH5 and CH6 pads.
// Users of these timers/pads must un-map the inverter assignment explicitly.
#define INVERTER_PIN_UART6      PC8 // Omnibus F4 V3 and later
#define INVERTER_PIN_UART3      PC9 // Omnibus F4 Pro Corners
#elif defined(EXUAVF4PRO)
#define INVERTER_PIN_UART6      PC8
#else
#define INVERTER_PIN_UART1      PC0 // DYS F4 Pro; Omnibus F4 AIO (1st gen) have a FIXED inverter on UART1
#endif

#define USE_EXTI

#define USE_ACC
#define USE_ACC_SPI_MPU6000

#define USE_GYRO
#define USE_GYRO_SPI_MPU6000

#define GYRO_1_CS_PIN           PA4
#define GYRO_1_SPI_INSTANCE     SPI1

// MPU6000 interrupts
#define USE_GYRO_EXTI
#define GYRO_1_EXTI_PIN         PC4
#define USE_MPU_DATA_READY_SIGNAL

#if defined(OMNIBUSF4SD)
#define GYRO_1_ALIGN            CW270_DEG
#elif defined(XRACERF4) || defined(EXUAVF4PRO)
#define GYRO_1_ALIGN            CW90_DEG
#elif defined(SYNERGYF4)
#define GYRO_1_ALIGN            CW0_DEG_FLIP
#else
#define GYRO_1_ALIGN            CW180_DEG
#endif

// Support for iFlight OMNIBUS F4 V3
// Has ICM20608 instead of MPU6000
// OMNIBUSF4SD is linked with both MPU6000 and MPU6500 drivers
#if defined (OMNIBUSF4SD) || defined(OMNIBUSF4BASE)
#define USE_ACC_SPI_MPU6500
#define USE_GYRO_SPI_MPU6500
#endif

// Dummy defines
#define GYRO_2_SPI_INSTANCE     GYRO_1_SPI_INSTANCE
#define GYRO_2_CS_PIN           NONE
#define GYRO_2_EXTI_PIN         NONE

#if !defined(SYNERGYF4) //No mag sensor on SYNERGYF4
#define USE_MAG
#define USE_MAG_HMC5883
#define USE_MAG_QMC5883
#define USE_MAG_LIS3MDL
#define MAG_HMC5883_ALIGN       CW90_DEG
#endif

#if !defined(SYNERGYF4) //No baro sensor on SYNERGYF4
#define USE_BARO
#if defined(OMNIBUSF4SD)
#define USE_BARO_SPI_BMP280
#define BARO_SPI_INSTANCE       SPI3
#define BARO_CS_PIN             PB3 // v1
#endif
#define USE_BARO_BMP085
#define USE_BARO_BMP280
#define USE_BARO_MS5611
#define BARO_I2C_INSTANCE       (I2CDEV_2)

#if defined(OMNIBUSF4SD)
#define DEFAULT_BARO_SPI_BMP280
#else
#define DEFAULT_BARO_BMP280
#endif
#endif

#define USE_MAX7456
#define MAX7456_SPI_INSTANCE    SPI3
#define MAX7456_SPI_CS_PIN      PA15

// Globally configure flashfs and drivers for various flash chips
#define USE_FLASHFS
#define USE_FLASH_M25P16
#define USE_FLASH_W25M512

#if defined(OMNIBUSF4SD)
#define ENABLE_BLACKBOX_LOGGING_ON_SDCARD_BY_DEFAULT
#define USE_SDCARD
#define USE_SDCARD_SPI
#define SDCARD_DETECT_INVERTED
#define SDCARD_DETECT_PIN               PB7
#define SDCARD_SPI_INSTANCE             SPI2
#define SDCARD_SPI_CS_PIN               SPI2_NSS_PIN
#define SPI2_TX_DMA_OPT                         0     // DMA 1 Stream 4 Channel 0

// For variants with SDcard replaced with flash chip
#define FLASH_CS_PIN            SDCARD_SPI_CS_PIN
#define FLASH_SPI_INSTANCE      SDCARD_SPI_INSTANCE

#elif defined(LUXF4OSD)
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT
#define FLASH_CS_PIN            PB12
#define FLASH_SPI_INSTANCE      SPI2

#else
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT
#define FLASH_CS_PIN            SPI3_NSS_PIN
#define FLASH_SPI_INSTANCE      SPI3
#endif // OMNIBUSF4

#ifdef OMNIBUSF4BASE
#define USE_RX_SPI
#define USE_RX_SPEKTRUM
#define USE_RX_SPEKTRUM_TELEMETRY
#define RX_CHANNELS_TAER
#define DEFAULT_RX_FEATURE      FEATURE_RX_SPI
#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_CYRF6936_DSM
#define RX_SPI_INSTANCE         SPI3
#define RX_NSS_PIN              PD2
#define RX_SPI_EXTI_PIN         PA0 // instead of rssi input
#endif

#define USE_VCP
#define USE_USB_DETECT
#define USB_DETECT_PIN   PC5

#define USE_UART1
#define UART1_RX_PIN            PA10
#define UART1_TX_PIN            PA9

#define USE_UART3
#define UART3_RX_PIN            PB11
#define UART3_TX_PIN            PB10

#if defined(EXUAVF4PRO)
#define USE_UART4
#define UART4_RX_PIN            PA1
#define UART4_TX_PIN            PA0
#endif

#define USE_UART6
#define UART6_RX_PIN            PC7
#define UART6_TX_PIN            PC6

#define USE_SOFTSERIAL1
#define USE_SOFTSERIAL2

#if defined(EXUAVF4PRO)
#define SERIAL_PORT_COUNT       7 // VCP, USART1, USART3, USART4, USART6, SOFTSERIAL x 2
#else
#define SERIAL_PORT_COUNT       6 // VCP, USART1, USART3, USART6, SOFTSERIAL x 2
#endif

#define USE_ESCSERIAL
#if defined(OMNIBUSF4SD)
#define ESCSERIAL_TIMER_TX_PIN  PB8  // (Hardware=0)
#else
#define ESCSERIAL_TIMER_TX_PIN  PB14 // (Hardware=0)
#endif

#define USE_SPI
#define USE_SPI_DEVICE_1

#if defined(OMNIBUSF4SD) || defined(LUXF4OSD)
#define USE_SPI_DEVICE_2
#define SPI2_NSS_PIN            PB12
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15
#endif

#define USE_SPI_DEVICE_3
#if defined(OMNIBUSF4SD)
  #define SPI3_NSS_PIN          PA15
#else
  #define SPI3_NSS_PIN          PB3
#endif
#define SPI3_SCK_PIN            PC10
#define SPI3_MISO_PIN           PC11
#define SPI3_MOSI_PIN           PC12

#define USE_I2C
#define USE_I2C_DEVICE_2
#define I2C2_SCL                NONE // PB10, shared with UART3TX
#define I2C2_SDA                NONE // PB11, shared with UART3RX
#if defined(OMNIBUSF4BASE) || defined(OMNIBUSF4SD)
#define USE_I2C_DEVICE_3
#define I2C3_SCL                NONE // PA8, PWM6
#define I2C3_SDA                NONE // PC9, CH6
#endif
#define I2C_DEVICE              (I2CDEV_2)

#define USE_ADC
#define ADC_INSTANCE            ADC2
#define ADC2_DMA_OPT            1  // DMA 2 Stream 3 Channel 1 (compat default)
//#define ADC_INSTANCE            ADC1
//#define ADC1_DMA_OPT            1  // DMA 2 Stream 4 Channel 0 (compat default)

#define CURRENT_METER_ADC_PIN   PC1  // Direct from CRNT pad (part of onboard sensor for Pro)
#define VBAT_ADC_PIN            PC2  // 11:1 (10K + 1K) divider
#ifdef DYSF4PRO
#define RSSI_ADC_PIN            PC3  // Direct from RSSI pad
#elif defined(OMNIBUSF4BASE)
#define RSSI_ADC_PIN            NONE
#else
#define RSSI_ADC_PIN            PA0  // Direct from RSSI pad
#endif

#define USE_TRANSPONDER

#define USE_RANGEFINDER
#define USE_RANGEFINDER_HCSR04
#define RANGEFINDER_HCSR04_TRIGGER_PIN     PA1
#define RANGEFINDER_HCSR04_ECHO_PIN        PA8
#define USE_RANGEFINDER_TF

#if defined(SYNERGYF4)
#define DEFAULT_FEATURES        (FEATURE_LED_STRIP | FEATURE_OSD | FEATURE_AIRMODE)
#else
#define DEFAULT_FEATURES        (FEATURE_OSD)
#endif

#define DEFAULT_VOLTAGE_METER_SOURCE VOLTAGE_METER_ADC
#define DEFAULT_CURRENT_METER_SOURCE CURRENT_METER_ADC

#define TARGET_IO_PORTA (0xffff & ~(BIT(14)|BIT(13)))
#define TARGET_IO_PORTB (0xffff & ~(BIT(2)))
#define TARGET_IO_PORTC (0xffff & ~(BIT(15)|BIT(14)|BIT(13)))
#define TARGET_IO_PORTD BIT(2)

#if defined(OMNIBUSF4SD) || defined(EXUAVF4PRO)
#define USABLE_TIMER_CHANNEL_COUNT 15
#define USED_TIMERS ( TIM_N(1) | TIM_N(2) | TIM_N(3) | TIM_N(4) | TIM_N(5) | TIM_N(10) | TIM_N(12) | TIM_N(8) | TIM_N(9))
#else
#define USABLE_TIMER_CHANNEL_COUNT 14
#define USED_TIMERS ( TIM_N(1) | TIM_N(2) | TIM_N(3) | TIM_N(5) | TIM_N(8) | TIM_N(12) )
#endif
#if defined(SYNERGYF4)
#define USE_PINIO
#define PINIO1_PIN              PB15 // VTX power switcher
#define USE_PINIOBOX
#endif


//------------------------------------------------------------------------------
// Resource changes needed by this particular target for software spi nrf24 rx
//------------------------------------------------------------------------------

// Look earlier in this file and you'll see these are defined. We need to undefine them
#undef USE_RX_SPEKTRUM
#undef USE_RX_SPEKTRUM_TELEMETRY
#undef RX_CHANNELS_TAER

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
#define RX_NSS_PIN              PB14
#define RX_SCK_PIN              PD2
#define RX_MISO_PIN             PA10
#define RX_MOSI_PIN             PA9

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
//#define RX_SPI_INSTANCE         SPI1
#ifdef RX_SPI_INSTANCE
#undef RX_SPI_INSTANCE
#endif

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
