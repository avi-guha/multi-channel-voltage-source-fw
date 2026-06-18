#ifndef __NO_OS_ESP32_SPI_H__
#define __NO_OS_ESP32_SPI_H__

#include <no_os_spi.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>


int32_t esp32_spi_init(struct no_os_spi_desc **desc, const struct no_os_spi_init_param *param);

int32_t esp32_spi_write_read(struct no_os_spi_desc *desc, uint8_t *, uint16_t);

int32_t esp32_spi_transfer(struct no_os_spi_desc *, struct no_os_spi_msg *, uint32_t);

int32_t esp32_spi_remove(struct no_os_spi_desc *);

int32_t esp32_spi_transfer_abort(struct no_os_spi_desc *);

#endif
