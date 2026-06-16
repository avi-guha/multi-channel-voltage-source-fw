#pragma once

extern "C" {
#include "ad717x.h"
#include "no_os_spi.h"
#include "spi_bus.h"
}

#include <cstdint>
#include "shared_spi_bus.hpp"

constexpr const uint8_t NUM_CHANNELS = 4;

class Adc{

  public:
    explicit Adc(SharedSpiBus& bus, const SpiDeviceConfig& spi_config){
      bus_ = bus;

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
        .max_speed_hz = 3000000,
        .chip_select = 0,
        .mode = NO_OS_SPI_MODE_3,
        .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
        .lanes = NO_OS_SPI_SINGLE_LANE,
        .platform_ops = &platform_ops,
        .platform_delays = platform_delays,
        .extra = ,
        .parent = NULL
      };

      ad717x_init_param init_param = {

        .num_regs = NUM_CHANNELS,
        .active_device = ad717x_device_type(ID_AD7172_2),
        .ref_en = true,
        .num_channels = NUM_CHANNELS,
        .mode = CONTINUOUS
      };

      // int32_t yo = AD717X_Init(&dev, init_param);
    };  

    static void Adc_task(void* arg);
    bool init();
    float readAdcChannel(uint8_t channel);

  private:
    SharedSpiBus& bus_;
    const SpiDeviceConfig& spi_config_;

};
