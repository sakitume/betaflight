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

// This file is copied with modifications from project Deviation,
// see http://deviationtx.com

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "platform.h"

#if defined(USE_RX_SPI) && defined(USE_RX_SOFTSPI)

#include "build/build_config.h"

#include "drivers/io.h"
#include "drivers/io_impl.h"
#include "drivers/system.h"

#include "pg/rx_spi.h"

#include "rx_spi.h"

#define DISABLE_RX()    {IOHi(DEFIO_IO(RX_NSS_PIN));}
#define ENABLE_RX()     {IOLo(DEFIO_IO(RX_NSS_PIN));}

#if defined(STM32F4) || defined(STM32F3)
    #define SOFT_SPI_IO_INPUT_CFG   IO_CONFIG(GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL)
    #define SOFT_SPI_IO_OUTPUT_CFG  IO_CONFIG(GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL)
#elif defined(STM32F7)
    #define SOFT_SPI_IO_INPUT_CFG   IO_CONFIG(GPIO_MODE_INPUT, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL)
    #define SOFT_SPI_IO_OUTPUT_CFG  IO_CONFIG(GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_HIGH, GPIO_NOPULL)
#elif defined(STM32F1)
    #define SOFT_SPI_IO_INPUT_CFG   IO_CONFIG(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz)
    #define SOFT_SPI_IO_OUTPUT_CFG  IO_CONFIG(GPIO_Mode_Out_PP,      GPIO_Speed_50MHz)
#endif

// Faster processors may need to stretch out the SCK pulse. I've noticed this with OMNIBUSF4 which uses
// an STM32F405 that runs at 168Mhz.
#if defined(STM32F405xx)
#define USE_SOFTSPI_DELAY
#endif

#if defined(USE_SOFTSPI_DELAY)
// Still getting occassional dropouts even with 13 nops
//#define SPI_DELAY()     { __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); }
// So try this instead
static volatile int hack;
#define     SPI_DELAY()     { hack = 2; while (hack--); }
#else
#define SPI_DELAY()     {}
#endif

void rxSpiDevicePreInit(const rxSpiConfig_t *rxSpiConfig)
{
    UNUSED(rxSpiConfig);
}

bool rxSpiDeviceInit(const rxSpiConfig_t *rxSpiConfig)
{
    UNUSED(rxSpiConfig);

    IOInit(DEFIO_IO(RX_NSS_PIN), OWNER_RX_SPI, 0);
    IOConfigGPIO(DEFIO_IO(RX_NSS_PIN), SOFT_SPI_IO_OUTPUT_CFG);

    IOInit(DEFIO_IO(RX_SCK_PIN), OWNER_RX_SPI, 0);
    IOConfigGPIO(DEFIO_IO(RX_SCK_PIN), SOFT_SPI_IO_OUTPUT_CFG);
    
    IOInit(DEFIO_IO(RX_MISO_PIN), OWNER_RX_SPI, 0);
    IOConfigGPIO(DEFIO_IO(RX_MISO_PIN), SOFT_SPI_IO_INPUT_CFG);
    
    IOInit(DEFIO_IO(RX_MOSI_PIN), OWNER_RX_SPI, 0);
    IOConfigGPIO(DEFIO_IO(RX_MOSI_PIN), SOFT_SPI_IO_OUTPUT_CFG);

    DISABLE_RX();
    return true;
}

// TODO: work in progress, replace some rxSpiTransferByte() calls to use trx_spi_write() instead
//------------------------------------------------------------------------------
static void trx_spi_write(int data)
{
#define __USE_METHOD_ONE__
#if defined(__USE_METHOD_ONE__)	
	for ( int i=7; i >= 0; --i ) 
    {
		IOLo(DEFIO_IO(RX_SCK_PIN));
		if ( ( data >> i ) & 1 ) 
        {
			IOHi(DEFIO_IO(RX_MOSI_PIN));
		}
        else 
        {
			IOLo(DEFIO_IO(RX_MOSI_PIN));
		}
		SPI_DELAY();
		IOHi(DEFIO_IO(RX_SCK_PIN));
		SPI_DELAY();
	}
	IOLo(DEFIO_IO(RX_SCK_PIN));
	IOLo(DEFIO_IO(RX_MOSI_PIN));
	SPI_DELAY();
#else
	for ( int i=7; i >= 0; --i ) 
    {
		IOLo(DEFIO_IO(RX_SCK_PIN));
		if ( ( data >> i ) & 1 ) 
        {
			IOHi(DEFIO_IO(RX_MOSI_PIN));
		}
        else 
        {
			IOLo(DEFIO_IO(RX_MOSI_PIN));
		}
		IOHi(DEFIO_IO(RX_SCK_PIN));
		SPI_DELAY();
	}
	IOLo(DEFIO_IO(RX_SCK_PIN));
	IOLo(DEFIO_IO(RX_MOSI_PIN));
#endif	
}

//------------------------------------------------------------------------------
static int trx_spi_read()
{
	int recv = 0;
	IOLo(DEFIO_IO(RX_MOSI_PIN));
	for ( int i = 7; i >= 0; --i ) 
    {
		recv = recv << 1;
		IOHi(DEFIO_IO(RX_SCK_PIN));
		SPI_DELAY();
		if ( IORead(DEFIO_IO(RX_MISO_PIN)) ) 
        {
			recv = recv | 1;
		}
		IOLo(DEFIO_IO(RX_SCK_PIN));
		SPI_DELAY();
	}
	return recv;
}

uint8_t rxSpiTransferByte(uint8_t data)
{
    if (data == 0xFF)
    {
        return trx_spi_read();
    }

    for(int i = 0; i < 8; i++) {

        if(data & 0x80)
        {
            IOHi(DEFIO_IO(RX_MOSI_PIN));
        }
        else
        {
            IOLo(DEFIO_IO(RX_MOSI_PIN));
        }

        IOHi(DEFIO_IO(RX_SCK_PIN));
        data <<= 1;
        SPI_DELAY();

        if(IORead(DEFIO_IO(RX_MISO_PIN)))
        {
            data |= 1;
        }

        IOLo(DEFIO_IO(RX_SCK_PIN));
        SPI_DELAY();
    }

    return data;
}

void rxSpiWriteByte(uint8_t data)
{
    ENABLE_RX();
    trx_spi_write(data);
    DISABLE_RX();
}

void rxSpiWriteCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    trx_spi_write(command);
    trx_spi_write(data);
    DISABLE_RX();
}

void rxSpiWriteCommandMulti(uint8_t command, const uint8_t *data, uint8_t length)
{
    ENABLE_RX();
    trx_spi_write(command);
    for (uint8_t i = 0; i < length; i++) {
        trx_spi_write(data[i]);
    }
    DISABLE_RX();
}

uint8_t rxSpiReadCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    trx_spi_write(command);
    const uint8_t ret = rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

void rxSpiReadCommandMulti(uint8_t command, uint8_t commandData, uint8_t *retData, uint8_t length)
{
    ENABLE_RX();
    trx_spi_write(command);
    for (uint8_t i = 0; i < length; i++) {
        retData[i] = rxSpiTransferByte(commandData);
    }
    DISABLE_RX();
}
#endif
