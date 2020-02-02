# Introduction

This documents describe a custom software spi RX implementation. I developed it to
experiment with using various micro sized transceivers based on an NRF24L01 or
on an LT8900 chip.

> Note: After doing everything described below, I was able to locate when the original
software spi layer was removed: git commit c60eecb. Reverting those changes is possibly
the same amount of work/change compared to the solution I came up with. Since my
implementation already works I'm going to stick with it. But be aware there is another
option worth considering.

I believe that a software spi rx implementation existed a while ago. I'm not certain
if it was in the official Cleanflight or Betaflight repositories or in a fork. I'll share some
links to some things I found on the internet:

* https://github.com/iforce2d/cleanflight/commit/f4ee4874190eaae06461555d1199b777730061ec
* https://github.com/iforce2d/cleanflight/commits/nrf24
* https://www.deviationtx.com/forum/protocol-development/5585-naze32-cleanflight-nrf24l01
* https://www.deviationtx.com/forum/protocol-development/5585-naze32-cleanflight-nrf24l01#45128

# NRF24L01 in Betaflight
I noticed that there were already several spi rx modules available in Betaflight that use
the NRF24L01. 

I stumbled across: `USE_RX_NRF24` in these files:

* `Betaflight\src\main\target\NUCLEOF446RE\target.h`
* `Betaflight\src\main\target\CJMCU\target.h`

By examining those header files you'll find a few more interesting defines.

```
#define USE_RX_CX10
#define USE_RX_H8_3D
#define USE_RX_INAV
#define USE_RX_SYMA
#define USE_RX_V202
```

From this I was able to see that there are a handful of rx modules using the NRF24L01
(as well as some that use it to emulate an XN297).

This sounds great and something I might be able to use *almost* "out of the box". However
the hardware I'm using to experiment with is a small whoop sized flight controller that only
has about 4 or 5 stm32 pins broken out onto pads that I can repurpose for SPI. These are
for things like the LED_STRIP, or TX1/RX1, or PPM input.

Those pads can be reconfigured for general purpose IO so that a software SPI implementation
could be used instead of the hardware peripheral SPI on the STM32 chips.

# A solution
I dove into the Betaflight 3.57 source code to see what approach I should take and had some
interesting options (such as replace `rx_nrf24l01.c` by not defining `USE_RX_NRF24` and use
an alternate version).

In the end I opted to replace the `rx_spi.c` body of code. Note that the "rx spi" system was
already broken out into 2 (identicially named) `rx_spi.c` files. One
located in `src/main/` and another (the low level SPI stuff) in `src/main/drivers/rx`.

I've edited `src/main/drivers/rx/rx_spi.c` so that it doesn't get compiled if `USE_RX_SOFTSPI` is defined.

> Note: `USE_RX_SOFTSPI` already existed and wasn't something I introduced. I discovered `USE_RX_SOFTSPI` when 
I stumbled across `bus_spi.c`. 

I've also created a new source file `src/main/drivers/rx/rx_softspi.c` which gets compiled when both
`USE_RX_SOFTSPI` and `USE_RX_SPI` are defined. It (of course) replaces `src/main/drivers/rx/rx_spi.c`
with a soft SPI implementation.

# Using it
You'll need to use my branch, or cherry pick from it. Since you're reading this I'm going to assume you'll
have no problem doing that.

Then you'll need to edit 2 files related to the target flight controller board you're using.

* `target.mk`
* `target.h`

For example, I'm using the "JHEMCU Play F4" whoop form factor flight controller. This is a `NOX` based target.
These are the two files I need to edit:

* `src/main/target/NOX/target.mk/target.mk`
* `src/main/target/NOX/target.mk/target.h`

Edit the `target.mk` and include the nrf24l01 and soft spi source files as well as the nrf 
rx source files you care about. This is how my file looked before the edit:

```
F411_TARGETS   += $(TARGET)
FEATURES       += VCP ONBOARDFLASH
TARGET_SRC = \
			drivers/accgyro/accgyro_mpu6500.c \
			drivers/accgyro/accgyro_spi_mpu6500.c \
			drivers/accgyro/accgyro_spi_mpu6000.c \
			drivers/barometer/barometer_bmp280.c \
			drivers/max7456.c
```

And how it looks with addition of the desired source files

```
F411_TARGETS   += $(TARGET)
FEATURES       += VCP ONBOARDFLASH
TARGET_SRC = \
			drivers/accgyro/accgyro_mpu6500.c \
			drivers/accgyro/accgyro_spi_mpu6500.c \
			drivers/accgyro/accgyro_spi_mpu6000.c \
			drivers/barometer/barometer_bmp280.c \
			drivers/max7456.c \
			drivers/rx/rx_nrf24l01.c \
			drivers/rx/rx_softspi.c \
			rx/nrf24_cx10.c \
			rx/nrf24_h8_3d.c \
			rx/nrf24_syma.c \
			rx/nrf24_v202.c
```

> Note: Be very careful with these make files. You must use an editor that uses real tab characters
when indenting the new lines. If you don't, the `make` program will have encounter an error.
This is just a quirk with how the `make` tool works.

Next you'll need to edit the `target.h` file. I've tried to make it easy by having two blocks of
code that you can paste onto the end of your `target.h` file.

The first block is for customzing (or undoing) what the existing `target.h` file may have configured.
It also has the section where you will specify which stm32 gpio pins you'll be using for software SPI
and also for "chip select" aka "slave select" for the NRF24L01.

The second block is for ensuring all of the relevant `USE_???` defines (as well as others) are
properly defined (or undefined).

> *IMPORTANT NOTE:* I'm hardwiring the CE pin of my NRF24L01 modules to VCC. This means it's always
enabled (in RX mode or TX mode). The existing `rx_nrf24l01.c` source code will enable/disable that
pin in a few places but for our purposes it isn't necessary. So I've also modified `rx_nrf24l01.c`
to do nothing wherever the code was previously enabling/disabling the CE pin; again, this change
only kicks in if `USE_RX_SOFTSPI` is defined *AND* you choose not to define `RX_CE_PIN`. If you feel
you really must wire up the CE pin of the module, then just be sure to define `RX_CE_PIN` to the
stm32 pin you wish to use. See the example snippet later on in this document for more details.

Below is what those two blocks look like for my modified NOX board. I literally copy/paste this
snippet to the end of my `src/main/target/NOX/target.mk/target.h` file, adjust the `target.mk`
file and then build with: `make -j12 TARGET=NOX`.

> Note: The snippet also works for an OMNIBUSF4 target board I used for development/testing 
(since it has SWD pins broken out). The only change I had to make was for the pins I chose for SPI.

Lastly, carefully review the pin (resource) assignments your flight controller board uses.
You can use the `resource` and `resource list` commands in the CLI of Betaflight Configurator
as well as examining the `target.h` file to see if there may be conflicts. Make any changes
as necessary. I recommend adjusting the snippet you'll be adding rather than the existing
contents of your header file. This lets you group all of the necessary changes into one 
(easy to review) section.

Oh yeah, I chose to setup the default RX protocol to: `RX_SPI_NRF24_V202_1M` as this
works perfectly. Some of the other nrf rx modules seem to have pitch, yaw and roll inverted.

Whoops....another thing. On OMNIBUSF4 I encountered some dropouts. I suspected that the
software SPI serial clock pulse might be too short due to the faster processor used
by that flight controller (an STM32F405). My NOX board (which uses an STM32F411) ran
perfectly fine however.

I added a slight delay to stretch out the clock pulse (look for `USE_SOFTSPI_DELAY` in source file `rx_softspi.c`. 
This fixed the issue on my OMNIBUSF4 board. I think it is worth noting here in this document
in case this needs to be revisited.

```c
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
#define RX_NSS_PIN              PB7
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
```
