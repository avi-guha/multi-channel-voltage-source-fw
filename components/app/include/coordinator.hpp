#pragma once

#include "channel.hpp"
#include "shared_spi_bus.hpp"
#include "ad717x.h"
#include "ad5761r.h"


static constexpr uint8_t NUM_CHANNELS = 4;
extern ad717x_st_reg ad7172_2_regs[];

class Coordinator{

  public:
    Coordinator();
    
    static void coordinator_task(void* arg);
    bool init();
    void run();


  private:
    spi_host_device_t host_ = SPI2_HOST;
    spi_bus_config_t bus_config_;
    no_os_spi_desc *spi_desc_;
    ad717x_dev *adc_dev_;
    ad5761r_dev *dac_dev_[NUM_CHANNELS];
    Channel channel_[NUM_CHANNELS];

};
