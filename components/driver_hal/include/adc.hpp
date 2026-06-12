#pragma once

extern "C" {
#include "ad717x.h"
}

#include <cstdint>
#include "shared_spi_bus.hpp"

constexpr const uint8_t NUM_CHANNELS = 4;

class Adc{

  public:
    explicit Adc(SharedSpiBus& bus, const SpiDeviceConfig& spi_config){
      bus_ = bus;

      ad717x_dev dev = {

        .num_regs = NUM_CHANNELS,
        .useCRC = ad717x_crc_mode(AD717X_USE_CRC),
        .active_device = ad717x_device_type(ID_AD7172_2),
        .ref_en = true,
        .num_channels = NUM_CHANNELS,
        .mode = CONTINUOUS
      }

      int32_t yo = AD717X_Init();
    };  

    static void Adc_task(void* arg);
    bool init();
    float readAdcChannel(uint8_t channel);

  private:
    SharedSpiBus& bus_;
    const SpiDeviceConfig& spi_config_;

};
