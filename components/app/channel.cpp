/**
 * @file channel.cpp
 * @brief Implementation of the Channel state machine (DAC set + ADC readback).
 */

#include <esp_log.h>
#include <esp_timer.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <math.h>
#include "channel.hpp"
#include "task_comms.hpp"


static constexpr const char* TAG = "Channel";

DataLog data;
extern UserCmd cmd;


Channel::Channel()
  : channel_id(0),
    mode(Mode::IDLE),
    done(false) {}


bool Channel::init(uint8_t id, ad717x_dev* adc_dev, ad5761r_dev* dac_dev){
    
  if (adc_dev == nullptr || dac_dev == nullptr) return false;

    channel_id = id;
    adc_dev_ = adc_dev;
    dac_dev_ = dac_dev;

    ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(0.0f));
    current_sense_data_ = load_calibration();

    return true;
}


void Channel::update(UserCmd& cmd){

  switch (cmd.mode){

    case Mode::IDLE:
      if (cmd.param.Cal.cal_en) set_calibration(cmd);
      else if (cmd.param.sps_setting) set_sps(cmd.param.sps_setting);

      if (mode != Mode::IDLE) {
        ESP_LOGI(TAG, "Ch%d stopped", channel_id);
        stop();
      }
      break;

    case Mode::SWEEP:
      mode = Mode::SWEEP;
      set_sweep_config(cmd);
      break;

    case Mode::STEADY:
      mode = Mode::STEADY;
      time_to_us(cmd);    
      steady_.voltage_ = cmd.param.Steady.voltage;
      steady_.initialized_ = false;
      break;
  }
}


void Channel::steady_run(){

  if(!steady_.initialized_){

    steady_.initialized_ = true;
    ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(steady_.voltage_));

    if(steady_.timer_en_){
      steady_.finish_time_ = esp_timer_get_time() + steady_.duration_us_; 
    }
  }

  vTaskDelay(pdMS_TO_TICKS(10));
  float current = get_current();

  data = {
    .channel_id = channel_id,
    .mode = mode,
    .voltage = steady_.voltage_,
    .current = current,
    .time = esp_timer_get_time() / 1e6f,
  };

  xQueueSend(data_queue, &data, 0);

  if (esp_timer_get_time() > steady_.finish_time_){
    ESP_LOGI(TAG, "Ch%d steady timer done", channel_id);
    stop();
  } 
}


void Channel::sweep_run(){

  ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(sweep_.voltage_in_V_));
  vTaskDelay(pdMS_TO_TICKS(10));
  float current = get_current();

  data = {
    .channel_id = channel_id,
    .mode = mode,
    .voltage = sweep_.voltage_in_V_,
    .current = current,
    .time = esp_timer_get_time() / 1e6f,
  };

  xQueueSend(data_queue, &data, 0);

  switch (sweep_.phase_){

    case SweepPhase::FIRST:

      if (sweep_.voltage_in_V_ < sweep_.range_in_V_){
        sweep_.voltage_in_V_ += sweep_.step_size_V_;
      }
      else{
        sweep_.phase_ = SweepPhase::SECOND;
      }
      break;

    case SweepPhase::SECOND:

      if (sweep_.voltage_in_V_ > - sweep_.range_in_V_){
        sweep_.voltage_in_V_ -= sweep_.step_size_V_;
      }
      else{
        sweep_.phase_ = SweepPhase::THIRD;
      }
      break;

    case SweepPhase::THIRD:

      if (sweep_.voltage_in_V_ < 0){
        sweep_.voltage_in_V_ += sweep_.step_size_V_;
      }
      else{
        ESP_LOGI(TAG, "Ch%d sweep done", channel_id);
        stop();
      }
      break;
  }
}


void Channel::stop(){

  ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(0.0f));
  done = true;
  mode = Mode::IDLE;

  data = {
    .channel_id = channel_id,
    .mode = mode,
  };
  xQueueSend(data_queue, &data, 0);

  ESP_LOGI(TAG,"CH%d Going back to standby", channel_id);
}


uint16_t Channel::voltage_to_bin(float voltage){
  return (uint16_t)((voltage / current_sense_data_.dac_vref + 2.0) * 65535.0 / 4.0);
}


float Channel::bin_to_voltage(uint32_t bin){
  return ADC_VREF * (static_cast<float>((static_cast<int32_t>(bin) - 0x800000)) 
      * static_cast<float>(0x400000) / ADC_GAIN) / (0.75 * DECI_24BIT);
}


float Channel::get_current(){
  uint32_t raw = 0;
  uint32_t data = 0;
  uint8_t rdy = 1;
  uint8_t ch_num = 0;

  do {
    AD717X_ReadData(adc_dev_, &adc_raw_data);
    raw = (uint32_t) adc_raw_data;
    data = (raw >> 8) & 0xFFFFFF;
    rdy = (raw & 0x80) >> 7;
    ch_num = raw & 0x0F;
  } while (rdy == 1 || ch_num != channel_id); 

  float opamp_gain = 1 + 50000.0 / current_sense_data_.r_gain;
  float current_in_uA = bin_to_voltage(data) / (current_sense_data_.r_1k * opamp_gain) * 1e6f;

  return current_in_uA;
}


void Channel::time_to_us(UserCmd& cmd){

  switch(cmd.param.Steady.time_unit){

    case TimeUnit::MIN:
      steady_.duration_us_ = (int64_t)((double)cmd.param.Steady.duration * 60.0 * 1e6f);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::HOUR:
      steady_.duration_us_ = (int64_t)((double)cmd.param.Steady.duration * 3600.0 * 1e6f);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::DAY:
      steady_.duration_us_ = (int64_t)((double)cmd.param.Steady.duration * 86400.0 * 1e6f);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::MONTH:
      steady_.duration_us_ = (int64_t)((double)cmd.param.Steady.duration * 2678400.0 * 1e6f);
      steady_.timer_en_ = true;
      break;

    default:
      steady_.timer_en_ = false;
      break;
  }
}


void Channel::set_sps(int sps){
int sps_options[] = {
  1, 2, 5, 10, 16, 20, 49, 59, 100, 200, 503, 1007
};
 
for (size_t i = 0; i < sizeof(sps_options); i++){
  if (sps == sps_options[i]){
    char buff[10];
    snprintf(buff, sizeof(buff), "sps_%d", sps);
    ad717x_configure_device_odr(adc_dev_, channel_id, *buff);

    return;
  }
}

}


void Channel::set_sweep_config(UserCmd& cmd){

  sweep_.phase_ = SweepPhase::FIRST; 
  sweep_.range_in_V_ = cmd.param.Sweep.range_in_V; 
  sweep_.step_size_V_ = cmd.param.Sweep.step_size / 1000;
  sweep_.single_sweep_steps_ = sweep_.range_in_V_ / sweep_.step_size_V_;
  sweep_.voltage_in_V_ = 0.0f;
}

auto Channel::load_calibration() -> current_sense_t {
  
  current_sense_t cal = { R_1K_DEFAULT, R_GAIN_DEFAULT, DAC_VREF_DEFAULT};

  char nvs_key[16];
  snprintf(nvs_key, sizeof(nvs_key), "cal_ch%u", channel_id);

  nvs_handle_t handle;
  esp_err_t err = nvs_open("calibration", NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Could not open NVS. Using default values");
    return cal;
  }

  size_t size = sizeof(cal);
  err = nvs_get_blob(handle, nvs_key, &cal, &size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "No previous calibration record for %s. Using default values", nvs_key);
    nvs_close(handle);
    return cal;
  }

  nvs_close(handle);
  return cal;
}


void Channel::set_calibration(UserCmd& cmd){

  float r1k = cmd.param.Cal.R_1k;
  float rg = cmd.param.Cal.R_gain;
  float vref = cmd.param.Cal.dac_vref;
  
  if(isnan(r1k) || isnan(rg) || isnan(vref)) {
    ESP_LOGE(TAG, "All values must be filled for a channel calibration");
    return;
  }

  if (r1k < R_1K_DEFAULT * (1.0 - R_1K_TOL) || r1k > R_1K_DEFAULT * (1.0 + R_1K_TOL)){
    ESP_LOGE(TAG, "1k resistor value is not within %.2f%% tolerance", R_1K_TOL * 100);
    return;
  }

  else if (rg < R_GAIN_DEFAULT * (1.0 - R_GAIN_TOL) || rg > R_GAIN_DEFAULT * (1.0 + R_GAIN_TOL)){
    ESP_LOGE(TAG, "Op-amp gain resistor value is not within %.2f%% tolerance", R_GAIN_TOL * 100);
    return;
  }

  else if (vref < DAC_VREF_DEFAULT * (1.0 - DAC_VREF_TOL) 
           || vref > DAC_VREF_DEFAULT * (1.0 + DAC_VREF_TOL)){
    ESP_LOGE(TAG, "DAC Vref is not within %.2f%% tolerance", DAC_VREF_TOL * 100);
    return;
  }

  current_sense_data_ = {.r_1k = r1k, .r_gain = rg, .dac_vref = vref};

  char nvs_key[16];
  snprintf(nvs_key, sizeof(nvs_key), "cal_ch%u", channel_id);

  nvs_handle_t handle;
  esp_err_t err = nvs_open("calibration",NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Could not open NVS with namespace");
    return;
  }

  err = nvs_set_blob(handle, nvs_key, &current_sense_data_, sizeof(current_sense_data_));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS content could not be set with key: %s", nvs_key);
    nvs_close(handle);
    return;
  }
  
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS could not save calibration value correctly");
    nvs_close(handle);
    return;
  }

  nvs_close(handle);
  ESP_LOGI(TAG, "Calibration successful");
}

