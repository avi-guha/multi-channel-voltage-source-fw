#include <cstdint>
#include "adc.hpp"
#include "ad717x.h"
#include "spi_bus.h"
#include "no_os_spi.h"


Adc::Adc(){}  


bool Adc::init(int sclk_pin, int miso_pin, int mosi_pin,
                         const SpiDeviceConfig* devices, size_t device_count){

  spi_bus_config_t bus_config = {};
  bus_config.sclk_io_num = sclk_pin;
  bus_config.mosi_io_num = mosi_pin;
  bus_config.miso_io_num = miso_pin;
  bus_config.quadwp_io_num = -1;
  bus_config.quadhd_io_num = -1;
  bus_config.max_transfer_sz = 32;

  ad717x_dev *dev;

  no_os_spi_desc *spi_desc;
  no_os_spi_platform_ops platform_ops = {
    .init = esp32_spi_init,
    .write_and_read = esp32_spi_write_read,
    .transfer = esp32_spi_transfer,
    .transfer_dma = NULL,
    .transfer_dma_async = NULL,
    .remove = esp32_spi_remove,
    .transfer_abort = esp32_spi_transfer_abort
  };

  no_os_platform_spi_delays platform_delays{
    .cs_delay_first = 0,
      .cs_delay_last = 0
  };

  no_os_spi_init_param spi_config_ = {
    .device_id = 5,
    .max_speed_hz = 8000000,
    .chip_select = 0,
    .mode = NO_OS_SPI_MODE_3,
    .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
    .lanes = NO_OS_SPI_SINGLE_LANE,
    .platform_ops = &platform_ops,
    .platform_delays = platform_delays,
    .extra = &bus_config,
    .parent = NULL
  };

  ad717x_init_param init_param = {

    .num_regs = NUM_CHANNELS,
    .active_device = ad717x_device_type(ID_AD7172_2),
    .ref_en = true,
    .num_channels = NUM_CHANNELS,
    .mode = CONTINUOUS
  };
  
  if (AD717X_Init(&dev, init_param) != 0){
   return false; 
  }

  return true;
}


float Adc::readAdcChannel(uint8_t channel){
  // AD717X_ReadData(&dev, );
  return 0.0;
}

