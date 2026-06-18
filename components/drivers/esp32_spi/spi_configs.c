#include "spi_configs.h"


spi_config_extra_t config_extra = {
  .host = SPI2_HOST,
  .bus = {
    .sclk_io_num = SPI_SCLK,
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = SPI_MISO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 32
  }
};


struct no_os_spi_platform_ops platform_ops = {
  .init = esp32_spi_init,
  .write_and_read = esp32_spi_write_read,
  .transfer = esp32_spi_transfer,
  .transfer_dma = NULL,
  .transfer_dma_async = NULL,
  .remove = esp32_spi_remove,
  .transfer_abort = esp32_spi_transfer_abort
};


struct no_os_spi_init_param spi_config = {
  .device_id = 5,
  .max_speed_hz = DAC_FREQ,
  .chip_select = 0,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .extra = &config_extra,
  .parent = NULL
};


ad717x_init_param adc_init_param = {
  .num_regs = NUM_CHANNELS,
  .active_device = ID_AD7172_2,
  .ref_en = true,
  .num_channels = NUM_CHANNELS,
  .mode = CONTINUOUS
};
