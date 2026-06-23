#pragma once

#include <no_os_spi.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>


int32_t esp32_spi_init(struct no_os_spi_desc **desc, const struct no_os_spi_init_param *param);

int32_t esp32_spi_write_read(struct no_os_spi_desc *desc, uint8_t *data, uint16_t data_len);

int32_t esp32_spi_remove(struct no_os_spi_desc *);
