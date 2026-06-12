#pragma once

#include "channel.hpp"
#include "hal/ad5761.hpp"
#include "hal/ad7172.hpp"
#include "hal/pin_config.hpp"
#include "hal/shared_spi_bus.hpp"


static constexpr uint8_t NUM_CHANNELS = 4;

class Coordinator{

  public:
    Coordinator();
    
    static void coordinator_task(void* arg);
    bool init();
    void run();


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

    Ad7172_2 adc_; 
    SharedSpiBus spi_bus_;
    Ad5761 dac_[NUM_CHANNELS];
    Channel channel_[NUM_CHANNELS];
    
    bool beginDac(Ad5761& dac, const SpiDeviceConfig& config);
};
