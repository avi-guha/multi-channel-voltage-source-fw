#include <esp_log.h>
#include "channel.hpp"
#include "task_comms.hpp"

static const char* TAG = "Channel";
struct UserCmd cmd;

Channel::Channel()
  : mode(Mode::OFF),
    done(false) 
    // Do spi bus initialization
{

  steady_.initialized_ = false;

  sweep_.phase_ = SweepPhase::FIRST;
  sweep_.step_count_ = 0;
}

void Channel::update(UserCmd& cmd){
  //switch here? with OFF state update like stopping?
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
  }
}

void Channel::steady_run(){

  if(!steady_.initialized_){
    steady_.initialized_ = true;
    //Set DAC
    steady_.finish_time_ = xTaskGetTickCount() + steady_.duration_in_ticks_; 
  }
  //Measure ADC
  if (xTaskGetTickCount() > steady_.finish_time_){
    stop();
  } 
}

void Channel::sweep_run(){

  switch (sweep_.phase_){

    //DAC set to step_count
    //Wait for some sec
    //ADC measure
    //xQueueSend

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
  //Set DAC to 0
  done = true;
  mode = Mode::OFF;
  //Make it so it also writes the channel id.
  ESP_LOGI(TAG,"Channel stopped, going back to standby mode.");
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

