#ifndef __SPI_CONFIGS_H__
#define __SPI_CONFIGS_H__

#include "ad717x.h"
#include "ad5761r.h"
#include "pin_config.h"
#include <driver/spi_master.h>
#include "no_os_spi.h"
#include "no_os_esp32_spi.h"

#define NUM_CHANNELS 4

typedef struct spi_config_extra {
  spi_host_device_t host;
  spi_bus_config_t bus; 
} spi_config_extra_t;

extern spi_config_extra_t config_extra;

extern struct no_os_spi_platform_ops platform_ops;

extern struct no_os_spi_init_param spi_config;

extern ad717x_init_param adc_init_param;

#endif
