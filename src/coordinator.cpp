#include <esp_log.h>
#include "coordinator.hpp"
#include "channel.hpp"

static const char* TAG = "Coordinator";

static Coordinator coordinator;
static Channel channel[NUM_CHANNELS];
static struct UserCmd cmd;

Coordinator::Coordinator()
    : channel_(), 
      dac1_(spi_bus_, kDac1SpiConfig),
      dac2_(spi_bus_, kDac2SpiConfig),
      dac3_(spi_bus_, kDac3SpiConfig),
      dac4_(spi_bus_, kDac4SpiConfig),
      adc_(spi_bus_, kAdcSpiConfig) {}


void Coordinator::coordinator_task(void* arg){

  if(coordinator.begin()){
    coordinator.run();
  }
  else{
    ESP_LOGE(TAG,"Failed to start");
  }
}

bool Coordinator::begin(){

  const SpiDeviceConfig devices[] = {
      kDac1SpiConfig,
      kDac2SpiConfig,
      kDac3SpiConfig,
      kDac4SpiConfig,
      kAdcSpiConfig,
  };

  if (!spi_bus_.begin(pins::kSpiSclk,
                      pins::kSpiMiso,
                      pins::kSpiMosi,
                      devices,
                      sizeof(devices) / sizeof(devices[0]))) {
    ESP_LOGE(TAG, "shared SPI bus init failed");
    return false;
  }

  if (!beginDac(dac1_, kDac1SpiConfig) || !beginDac(dac2_, kDac2SpiConfig) ||
      !beginDac(dac3_, kDac3SpiConfig) || !beginDac(dac4_, kDac4SpiConfig)) {
    return false;
  }

  if (!adc_.begin()) {
    ESP_LOGE(TAG, "%s init failed", kAdcSpiConfig.name);
    return false;
  }

  ESP_LOGI(TAG, "startup complete: 4 DACs + 1 ADC");
  return true;
}

void Coordinator::run(){

  while(true){ 

    while(xQueueReceive(user_cmd_queue, &cmd, 0) == pdTRUE){
      channel[cmd.channel_id].update(cmd);
    }

    for(int i = 0; i < NUM_CHANNELS; i++){

      switch(channel[i].mode){

        case Mode::OFF:
          break;

        case Mode::STEADY:
          channel[i].steady_run();
          break;

        case Mode::SWEEP:
          channel[i].sweep_run();
          break;
      }
    }
  }
}

bool Coordinator::beginDac(Ad5761& dac, const SpiDeviceConfig& config) {

  if (!dac.begin()) {
    ESP_LOGE(TAG, "%s init failed", config.name);
    return false;
  }

  return true;
}
