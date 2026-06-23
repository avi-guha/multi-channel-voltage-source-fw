#pragma once

#include <cstdint>
#include "shared_spi_bus.hpp"

extern "C" {
#include "spi_bus.h"
}


constexpr const uint8_t NUM_CHANNELS = 4;

class Adc{

  public:
    explicit Adc();  

    bool init(int sclk_pin, int miso_pin, int mosi_pin,
                         const SpiDeviceConfig* devices, size_t device_count);
    float readAdcChannel(uint8_t channel);

  private:
 struct esp32_spi_config {
    
 }; 

};
