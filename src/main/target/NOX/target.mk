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
