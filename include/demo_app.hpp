#pragma once

#include "ad5761.hpp"
#include "ad7172.hpp"
#include "pin_config.hpp"
#include "shared_spi_bus.hpp"

class DemoApp {
 public:
  DemoApp();

  bool begin();
  void runOnce();

 private:
  static constexpr SpiDeviceConfig kDacSpiConfig{
      "AD5761",
      pins::kDacCs,
      app_config::kDacClockHz,
      SpiMode::kMode2,
  };

  static constexpr SpiDeviceConfig kAdcSpiConfig{
      "AD7172-2",
      pins::kAdcCs,
      app_config::kAdcClockHz,
      SpiMode::kMode3,
  };

  SharedSpiBus spi_bus_;
  Ad5761 dac_;
  Ad7172_2 adc_;
  size_t sweep_index_ = 0;
};
