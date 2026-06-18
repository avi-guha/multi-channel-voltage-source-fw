#ifndef __SPI_CONFIGS_H__
#define __SPI_CONFIGS_H__

#include "ad717x.h"
#include "ad5761r.h"
#include "no_os_spi.h"
#include <driver/spi_master.h>

#define NUM_CHANNELS 4

typedef struct spi_config_extra {
  spi_host_device_t host;
  spi_bus_config_t bus; 
} spi_config_extra_t;

extern spi_config_extra_t config_extra;

extern struct no_os_spi_platform_ops platform_ops;

extern struct no_os_spi_init_param spi_config;

extern ad717x_init_param adc_init_param;

extern ad717x_st_reg ad7172_2_regs[];
extern const uint8_t ad7172_2_num_regs;
extern struct ad717x_channel_map chan_map[NUM_CHANNELS];
extern struct ad717x_channel_setup setup[NUM_CHANNELS];

uint32_t pga[NUM_CHANNELS];
extern struct ad717x_filtcon filtcon[NUM_CHANNELS];

#endif
