#pragma once

#include "ad717x.h"
#include "ad5761r.h"
#include "channel.hpp"


static constexpr uint8_t NUM_CHANNELS = 4;

class Coordinator{

  public:
    Coordinator();
    
    static void coordinator_task(void* arg);
    bool init();
    void run();


  private:
    no_os_spi_desc *spi_desc_;
    ad717x_dev *adc_dev_;
    ad5761r_dev *dac_dev_[NUM_CHANNELS];
    Channel channel_[NUM_CHANNELS];
};
