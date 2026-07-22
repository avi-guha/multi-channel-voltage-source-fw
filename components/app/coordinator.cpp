/**
 * @file coordinator.cpp
 * @brief Implementation of the top-level coordinator (hardware init + run loop).
 */

#include <esp_log.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include "coordinator.hpp"
#include "channel.hpp"
#include "ad717x.h"
#include "ad5761r.h"
#include "pin_config.h"
#include "spi_configs.h"

static constexpr const char* TAG = "Coordinator";

extern UserCmd cmd;
static Coordinator coordinator;


Coordinator::Coordinator() {}


void Coordinator::coordinator_task(void* arg){

  if(coordinator.init()){
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));
    coordinator.run();
  }
  else{
    ESP_LOGE(TAG,"Failed to start");
    vTaskDelete(nullptr);
  }
}


bool Coordinator::init(){

  gpio_config(&cs_high);
  gpio_set_level(static_cast<gpio_num_t>(CS_ADC) , 1);
  gpio_set_level(static_cast<gpio_num_t>(CS_DAC0), 1);
  gpio_set_level(static_cast<gpio_num_t>(CS_DAC1), 1);
  gpio_set_level(static_cast<gpio_num_t>(CS_DAC2), 1);
  gpio_set_level(static_cast<gpio_num_t>(CS_DAC3), 1);

  const esp_err_t init_result = 
    spi_bus_initialize(spi_config.host, &spi_config.bus, SPI_DMA_DISABLED);
  if ((init_result != ESP_OK) && (init_result != ESP_ERR_INVALID_STATE)) {
    ESP_LOGE(TAG, "shared SPI bus init failed");
    return false;
  }

  ad5761r_init_param dac_init_param = {
    .spi_init = *dac_config[0],
    .type = AD5761R,
    .out_range = AD5761R_RANGE_M_5V_TO_P_5V,
    .pwr_voltage = AD5761R_SCALE_HALF,
    .clr_voltage = AD5761R_SCALE_HALF,
    .int_ref_en = true,
    .exc_temp_sd_en = true,
    .b2c_range_en = false,
    .ovr_en = false,
    .daisy_chain_en = true,
  };

  ad717x_init_param adc_init_param = {
    .spi_init = adc_config,
    .regs = ad7172_2_regs, 
    .num_regs = ad7172_2_num_regs,
    .active_device = ID_AD7172_2,
    .ref_en = true,
    .num_channels = NUM_CHANNELS,
    .num_setups = NUM_CHANNELS,
    .chan_map = {chan_map[0], chan_map[1], chan_map[2], chan_map[3]},
    .setups = {setup[0], setup[1], setup[2], setup[3]},
    .filter_configuration = {filtcon[0], filtcon[1], filtcon[2], filtcon[3]},
    .mode = INTERNAL_OFFSET_CALIB
  };

  if (AD717X_Init(&adc_dev_, adc_init_param) < 0){
    ESP_LOGE(TAG, "ADC init failed");
    return false;
  }

  for (int i = 0; i < NUM_CHANNELS; i++){

    dac_init_param.spi_init = *dac_config[i]; 
    if (ad5761r_init(&dac_dev_[i], dac_init_param) < 0 ){
      ESP_LOGE(TAG, "DAC %d failed to initialize", i);
      return false;
    }
    
    ad5761r_software_full_reset(dac_dev_[i]); //Avoids edge case brownout triggering on boot
    ad5761r_config(dac_dev_[i]);

    if (!channel_[i].init(i, adc_dev_, dac_dev_[i])){
      ESP_LOGE(TAG, "Channel %d failed to initialize", i);
      return false;
    }
  }  

  //Appends 1 byte statistics to ADC readout. 
  //Used to identify channel ID for given adc data
  ad717x_st_reg *ifmode = AD717X_GetReg(adc_dev_, AD717X_IFMODE_REG);
  ifmode->value |= AD717X_IFMODE_REG_DATA_STAT;
  if (AD717X_WriteRegister(adc_dev_, AD717X_IFMODE_REG) < 0) {
    ESP_LOGE(TAG, "IFMODE write failed");
    return false;
  }

  vTaskDelay(pdMS_TO_TICKS(50));

  AD717X_WaitForReady(adc_dev_, AD717X_CONV_TIMEOUT);
  ad717x_st_reg *adcmode = AD717X_GetReg(adc_dev_, AD717X_ADCMODE_REG);
  adcmode->value &= ~AD717X_ADCMODE_REG_MODE_MSK;
  adcmode->value |= AD717X_ADCMODE_REG_MODE(INTERNAL_GAIN_CALIB);
  if (AD717X_WriteRegister(adc_dev_, AD717X_ADCMODE_REG) < 0) {
    ESP_LOGE(TAG, "ADC Internal gain calibration failed");
    return false;
  }

  AD717X_WaitForReady(adc_dev_, AD717X_CONV_TIMEOUT);
  adcmode->value &= ~AD717X_ADCMODE_REG_MODE_MSK;
  adcmode->value |= AD717X_ADCMODE_REG_MODE(SYS_OFFSET_CALIB);
  if (AD717X_WriteRegister(adc_dev_, AD717X_ADCMODE_REG) < 0) {
    ESP_LOGE(TAG, "ADC System offset calibration failed");
    return false;
  }

  AD717X_WaitForReady(adc_dev_, AD717X_CONV_TIMEOUT);
  adcmode->value &= ~AD717X_ADCMODE_REG_MODE_MSK;
  adcmode->value |= AD717X_ADCMODE_REG_MODE(CONTINUOUS);
  if (AD717X_WriteRegister(adc_dev_, AD717X_ADCMODE_REG) < 0) {
    ESP_LOGE(TAG, "Continuous mode transition failed");
    return false;
  }

  vTaskDelay(pdMS_TO_TICKS(60));
  ESP_LOGI(TAG, "startup complete");
  return true;
}

void Coordinator::run(){

  while(true){ 

    while(xQueueReceive(user_cmd_queue, &cmd, 1) == pdTRUE){
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

    esp_task_wdt_reset();
  }
}

