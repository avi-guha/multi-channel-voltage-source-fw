#include <esp_log.h>
#include "coordinator.hpp"
#include "channel.hpp"

static const char* TAG = "Coordinator";

static Coordinator coordinator;
UserCmd cmd;


Coordinator::Coordinator() : adc_(spi_bus_, kAdcSpiConfig) {}


void Coordinator::coordinator_task(void* arg){

  if(coordinator.init()){
    coordinator.run();
  }
  else{
    ESP_LOGE(TAG,"Failed to start");
  }
}


bool Coordinator::init(){

  const SpiDeviceConfig devices[] = {
      kDac1SpiConfig,
      kDac2SpiConfig,
      kDac3SpiConfig,
      kDac4SpiConfig,
      kAdcSpiConfig,
  };

  if (!spi_bus_.begin(pins::kSpiSclk, pins::kSpiMiso, pins::kSpiMosi,
                      devices, sizeof(devices) / sizeof(devices[0]))) {
    ESP_LOGE(TAG, "shared SPI bus init failed");
    return false;
  }

  if (!adc_.begin()) {
    ESP_LOGE(TAG, "%s init failed", kAdcSpiConfig.name);
    return false;
  }

  for (int i = 0; i < NUM_CHANNELS; i++){

    if (!dac_[i].begin(&spi_bus_, &devices[i]) ){
      ESP_LOGE(TAG, "DAC %d failed to initialize", i);
      return false;
    }

    if (!channel_[i].init(i, &adc_, &dac_[i])){
      ESP_LOGE(TAG, "Channel %d failed to initialize", i);
      return false;
    }
  }  
  

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


// bool Coordinator::beginDac(Ad5761& dac, const SpiDeviceConfig& config) {
//
//   if (!dac.begin()) {
//     ESP_LOGE(TAG, "%s init failed", config.name);
//     return false;
//   }
//
//   return true;
// }


