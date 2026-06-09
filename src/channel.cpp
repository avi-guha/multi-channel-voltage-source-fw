#include <esp_log.h>
#include "channel.hpp"
#include "task_comms.hpp"

static const char* TAG = "Channel";

UserCmd cmd;
DataLog data;


Channel::Channel()
  : channel_id(0),
    mode(Mode::OFF),
    done(false),
    adc_(nullptr),
    dac_(nullptr) {}


bool Channel::init(uint8_t id, Ad7172_2* adc, Ad5761* dac){

  if (adc == nullptr || dac == nullptr) return false;

    adc_ = adc;
    dac_ = dac;
    channel_id = id;
    return true;
}


void Channel::update(UserCmd& cmd){

  switch (cmd.mode){

    case Mode::OFF:
      if (mode != Mode::OFF){
        stop();
        mode = Mode::OFF;
      }
      break;

    case Mode::SWEEP:

      mode = Mode::SWEEP;
      sweep_.phase_ = SweepPhase::FIRST; 
      sweep_steps_config(cmd);
      sweep_.step_count_ = 0;
      break;

    case Mode::STEADY:

      mode = Mode::STEADY;
      time_to_xtickcount(cmd);    
      steady_.initialized_ = false;
  }
}


void Channel::steady_run(){

  if(!steady_.initialized_){
    steady_.initialized_ = true;
    dac_ -> setDACVoltage(steady_.voltage_);
    steady_.finish_time_ = xTaskGetTickCount() + steady_.duration_in_ticks_; 
  }

  float current = adc_ -> readADCChannel(channel_id);

  data = {
    .channel_id = channel_id,
    .mode = mode,
    .voltage = steady_.voltage_,
    .current = current,
    .time = pdTICKS_TO_MS(xTaskGetTickCount()),
  };

  xQueueSend(data_queue, &data, 0);

  if (xTaskGetTickCount() > steady_.finish_time_){
    stop();
  } 
}


void Channel::sweep_run(){

    dac_ -> setDACVoltage(sweep_.step_count_);
    vTaskDelay(pdMS_TO_TICKS(5));
    float current = adc_ -> readADCChannel(channel_id);
     
    data = {
      .channel_id = channel_id,
      .mode = mode,
      .voltage = sweep_.step_count_,
      .current = current,
      .time = pdTICKS_TO_MS(xTaskGetTickCount()),
     };

    xQueueSend(data_queue, &data, 0);

  switch (sweep_.phase_){


    case SweepPhase::FIRST:

      if (sweep_.step_count_ < sweep_.range_in_mV_){
        sweep_.step_count_ += sweep_.step_size_;
      }
      else{
        sweep_.phase_ = SweepPhase::SECOND;
      }
      break;

    case SweepPhase::SECOND:

      if (sweep_.step_count_ > - sweep_.range_in_mV_){
        sweep_.step_count_ -= sweep_.step_size_;
      }
      else{
        sweep_.phase_ = SweepPhase::THIRD;
      }
      break;

    case SweepPhase::THIRD:

      if (sweep_.step_count_ < 0){
        sweep_.step_count_ += sweep_.step_size_;
      }
      else{
        stop();
      }
      break;
  }
}


void Channel::stop(){

  dac_ -> setDACVoltage(0.0f);
  done = true;
  mode = Mode::OFF;
  //Make it so it also writes the channel id.
  ESP_LOGI(TAG,"Channel %d stopped, going back to standby mode.", channel_id);
}


void Channel::time_to_xtickcount(UserCmd& cmd){

  switch(cmd.time_unit){
    // Converts human readable time to FreeRTOS's xtasktick_count 
    case TimeUnit::Sec:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.duration * 1000);
      break;

    case TimeUnit::Min:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.duration * 60000);
      break;

    case TimeUnit::Hour:
      steady_.duration_in_ticks_ = pdMS_TO_TICKS(cmd.duration * 360000);
      break;
  }

}


void Channel::sweep_steps_config(UserCmd& cmd){

  sweep_.range_in_mV_ = cmd.param.Sweep.range_in_V * 1000; 
  sweep_.step_size_ = cmd.param.Sweep.step_size;

  sweep_.single_sweep_steps_ = sweep_.range_in_mV_ / sweep_.step_size_;
}

