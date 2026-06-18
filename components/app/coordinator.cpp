#include <esp_log.h>
#include "coordinator.hpp"
#include "channel.hpp"
#include "ad717x.h"
#include "ad5761r.h"
#include "spi_configs.h"

static constexpr const char* TAG = "Coordinator";

extern UserCmd cmd;
static Coordinator coordinator;


Coordinator::Coordinator() {}


void Coordinator::coordinator_task(void* arg){

  if(coordinator.init()){
    coordinator.run();
  }
  else{
    ESP_LOGE(TAG,"Failed to start");
  }
}


bool Coordinator::init(){

  if (no_os_spi_init(&spi_desc_, &spi_config) < 0){
    ESP_LOGE(TAG, "shared SPI bus init failed");
    return false;
  }

  
  if (AD717X_Init(&adc_dev_, adc_init_param) < 0){
    ESP_LOGE(TAG, "ADC init failed");
    return false;
  }

  // 
  // if (!adc_.begin()) {
  //   ESP_LOGE(TAG, "%s init failed", kAdcSpiConfig.name);
  //   return false;
  // }
  //
  // for (int i = 0; i < NUM_CHANNELS; i++){
  //
  //   if (!dac_[i].begin(&spi_bus_, &devices[i]) ){
  //     ESP_LOGE(TAG, "DAC %d failed to initialize", i);
  //     return false;
  //   }
  //
  //   if (!channel_[i].init(i, &adc_, &dac_[i])){
  //     ESP_LOGE(TAG, "Channel %d failed to initialize", i);
  //     return false;
  //   }
  // }  

  ESP_LOGI(TAG, "startup complete");
  return true;
}


void Coordinator::run(){

  while(true){ 

    while(xQueueReceive(user_cmd_queue, &cmd, 0) == pdTRUE){
      channel_[cmd.channel_id].update(cmd);
    }

    for(int i = 0; i < NUM_CHANNELS; i++){

      switch(channel_[i].mode){

        case Mode::OFF:
          break;

        case Mode::STEADY:
          channel_[i].steady_run();
          break;

        case Mode::SWEEP:
          channel_[i].sweep_run();
          break;
      }
    }
  }
}

