#pragma once

#include "ad717x.h"
#include "ad5761r.h"
#include "no_os_spi.h"
#include <driver/spi_master.h>

#define NUM_CHANNELS 4

/*
 * no OS SPI config
 */
typedef struct {
  spi_host_device_t host;
  spi_bus_config_t bus;
} esp32_spi_config_t;


extern esp32_spi_config_t spi_config;
extern struct no_os_spi_platform_ops platform_ops;


/*
 * DAC SPI config
 */
extern struct no_os_spi_init_param dac0_config;

extern struct no_os_spi_init_param dac1_config;

extern struct no_os_spi_init_param dac2_config;

extern struct no_os_spi_init_param dac3_config;

extern struct no_os_spi_init_param *dac_config[NUM_CHANNELS];


/*
 * ADC SPI config
 */
extern struct no_os_spi_init_param adc_config;

extern ad717x_init_param adc_init_param;

extern ad717x_st_reg ad7172_2_regs[];

extern const uint8_t ad7172_2_num_regs;

extern struct ad717x_channel_map chan_map[NUM_CHANNELS];

extern struct ad717x_channel_setup setup[NUM_CHANNELS];

extern uint32_t pga[NUM_CHANNELS];

extern struct ad717x_filtcon filtcon[NUM_CHANNELS];

