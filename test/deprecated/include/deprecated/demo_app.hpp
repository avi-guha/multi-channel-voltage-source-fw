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
  static constexpr SpiDeviceConfig kDac1SpiConfig{
      "AD5761-1",
      pins::kDacCs1,
      app_config::kDacClockHz,
      SpiMode::kMode2,
  };

  static constexpr SpiDeviceConfig kDac2SpiConfig{
      "AD5761-2",
      pins::kDacCs2,
      app_config::kDacClockHz,
      SpiMode::kMode2,
  };

  static constexpr SpiDeviceConfig kDac3SpiConfig{
      "AD5761-3",
      pins::kDacCs3,
      app_config::kDacClockHz,
      SpiMode::kMode2,
  };

  static constexpr SpiDeviceConfig kDac4SpiConfig{
      "AD5761-4",
      pins::kDacCs4,
      app_config::kDacClockHz,
      SpiMode::kMode2,
  };

  static constexpr SpiDeviceConfig kAdcSpiConfig{
      "AD7172-2",
      pins::kAdcCs,
      app_config::kAdcClockHz,
      SpiMode::kMode3,
  };

  bool beginDac(Ad5761& dac, const SpiDeviceConfig& config);
  void setAllDacVoltages(float voltage);

  SharedSpiBus spi_bus_;
  Ad5761 dac1_;
  Ad5761 dac2_;
  Ad5761 dac3_;
  Ad5761 dac4_;
  Ad7172_2 adc_;
  size_t sweep_index_ = 0;
};
