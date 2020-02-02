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
#define SPI_DELAY()     { __asm__ __volatile__("nop"); __asm__ __volatile__("nop"); __asm__ __volatile__("nop"); }
#else
#define SPI_DELAY()     {}
#endif

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

uint8_t rxSpiTransferByte(uint8_t data)
{
    for(int i = 0; i < 8; i++) {

        if(data & 0x80)
        {
            IOHi(DEFIO_IO(RX_MOSI_PIN));
        }
        else
        {
            IOLo(DEFIO_IO(RX_MOSI_PIN));
        }

        SPI_DELAY();
        IOHi(DEFIO_IO(RX_SCK_PIN));
        data <<= 1;


        if(IORead(DEFIO_IO(RX_MISO_PIN)))
        {
            data |= 1;
        }

        SPI_DELAY();
        IOLo(DEFIO_IO(RX_SCK_PIN));
    }

    return data;
}

uint8_t rxSpiWriteByte(uint8_t data)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiWriteCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiWriteCommandMulti(uint8_t command, const uint8_t *data, uint8_t length)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    for (uint8_t i = 0; i < length; i++) {
        rxSpiTransferByte(data[i]);
    }
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiReadCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    rxSpiTransferByte(command);
    const uint8_t ret = rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiReadCommandMulti(uint8_t command, uint8_t commandData, uint8_t *retData, uint8_t length)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    for (uint8_t i = 0; i < length; i++) {
        retData[i] = rxSpiTransferByte(commandData);
    }
    DISABLE_RX();
    return ret;
}
#endif
