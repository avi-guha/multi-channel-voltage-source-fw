#include <esp_log.h>
#include "channel.hpp"
#include "task_comms.hpp"


static constexpr const char* TAG = "Channel";

extern UserCmd cmd;
extern DataLog data;


Channel::Channel()
  : channel_id(0),
    mode(Mode::OFF),
    done(false) {}


bool Channel::init(uint8_t id, ad717x_dev* adc_dev, ad5761r_dev* dac_dev){
    
  if (adc_dev == nullptr || dac_dev == nullptr) return false;

    channel_id = id;
    adc_dev_ = adc_dev;
    dac_dev_ = dac_dev;

    ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(0.0));

    return true;
}


void Channel::update(UserCmd& cmd){

  switch (cmd.mode){
    case Mode::OFF:
      if (mode != Mode::OFF) stop();
      break;

    case Mode::SWEEP:

      mode = Mode::SWEEP;
      sweep_.phase_ = SweepPhase::FIRST; 
      sweep_steps_config(cmd);
      sweep_.voltage_ = 0;
      break;

    case Mode::STEADY:
      mode = Mode::STEADY;
      time_to_xtickcount(cmd);    
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
      steady_.finish_time_ = xTaskGetTickCount() + steady_.duration_in_ticks_; 
    }
  }

  data = {
    .channel_id = channel_id,
    .mode = mode,
    .voltage = steady_.voltage_,
    .current = get_current(),
    .time = pdTICKS_TO_MS(xTaskGetTickCount()),
  };

  xQueueSend(data_queue, &data, 0);

  if (xTaskGetTickCount() > steady_.finish_time_){
    stop();
  } 
}



void Channel::sweep_run(){

  ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(sweep_.voltage_));
  vTaskDelay(pdMS_TO_TICKS(2));
  
  AD717X_WaitForReady(adc_dev_, 2);
  AD717X_ReadData(adc_dev_, &voltage_read);
  float current = (float)(voltage_read / R_1K);

  data = {
    .channel_id = channel_id,
    .mode = mode,
    .voltage = sweep_.voltage_,
    .current = current,
    .time = pdTICKS_TO_MS(xTaskGetTickCount()),
  };

    xQueueSend(data_queue, &data, 0);

  switch (sweep_.phase_){


    case SweepPhase::FIRST:

      if (sweep_.voltage_ < sweep_.range_in_mV_){
        sweep_.voltage_ += sweep_.step_size_;
      }
      else{
        sweep_.phase_ = SweepPhase::SECOND;
      }
      break;

    case SweepPhase::SECOND:

      if (sweep_.voltage_ > - sweep_.range_in_mV_){
        sweep_.voltage_ -= sweep_.step_size_;
      }
      else{
        sweep_.phase_ = SweepPhase::THIRD;
      }
      break;

    case SweepPhase::THIRD:

      if (sweep_.voltage_ < 0){
        sweep_.voltage_ += sweep_.step_size_;
      }
      else{
        stop();
      }
      break;
  }
}


void Channel::stop(){
  ad5761r_write_update_dac_register(dac_dev_, voltage_to_bin(0.0f));
  done = true;
  mode = Mode::OFF;
  ESP_LOGI(TAG,"Ch%d stopped, going back to standby.", channel_id);
}


void Channel::time_to_xtickcount(UserCmd& cmd){

  switch(cmd.param.Steady.time_unit){

    case TimeUnit::MIN:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.param.Steady.duration * 60000);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::HOUR:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.param.Steady.duration * 360000);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::DAY:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.param.Steady.duration * 360000 * 24);
      steady_.timer_en_ = true;
      break;

    case TimeUnit::MONTH:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.param.Steady.duration * 360000 * 24 * 31);
      steady_.timer_en_ = true;
      break;

    default:
      steady_.timer_en_ = false;
      break;
  }

}


uint16_t Channel::voltage_to_bin(float voltage){
  return (uint16_t)((voltage / 2.497 + 2.0) * 65535.0 / 4.0);
}


float Channel::bin_to_voltage(uint32_t bin){
  return ADC_VREF * (static_cast<float>((static_cast<int32_t>(bin) - 0x800000)) * static_cast<float>(0x400000) / ADC_GAIN) / (0.75 * DECI_24BIT);
}

float Channel::get_current(){
  uint32_t raw = 0;
  uint32_t data = 0;
  uint8_t ch_num = 0;

  do {
    AD717X_ReadData(adc_dev_, &voltage_read);
    raw = (uint32_t) voltage_read;
    data = (raw >> 8) & 0xFFFFFF;
    ch_num = raw & 0x0F;
  } while(ch_num != channel_id);

  float current_in_uA = bin_to_voltage(data) / (R_1K * OPAMP_GAIN) * 1e6f;

  ESP_LOGI(TAG, "Channel%d, Amplified voltage: %f V, Current: %f uA", ch_num, bin_to_voltage(data), current_in_uA);
  return current_in_uA;
}

void Channel::sweep_steps_config(UserCmd& cmd){

  sweep_.range_in_mV_ = cmd.param.Sweep.range_in_V * 1000; 
  sweep_.step_size_ = cmd.param.Sweep.step_size;

  sweep_.single_sweep_steps_ = sweep_.range_in_mV_ / sweep_.step_size_;
}

