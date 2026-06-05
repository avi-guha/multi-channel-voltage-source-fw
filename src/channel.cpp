#include "channel.hpp"
#include "task_comms.hpp"

struct UserCmd cmd;

Channel::Channel()
  : mode(Mode::OFF),
    done(false) 
    // Do spi bus initialization
   {
    steady_.initialized_ = false;
    sweep_.step_count_ = 0;
   }

void Channel::update(UserCmd& cmd){

 steady_.initialized_ = false;
 done = false;

 if(cmd.mode == Mode::SWEEP){
   mode = Mode::SWEEP;
   sweep_steps_config(cmd);
   sweep_.step_count_ = 0;
 }
 else{

  mode = Mode::STEADY;
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

} 

void Channel::steady_run(){

  if(!steady_.initialized_){
     steady_.initialized_ = true;
    steady_.finish_time_ = xTaskGetTickCount() + steady_.duration_in_ticks_; 
  }
  else if (xTaskGetTickCount() > steady_.finish_time_){
    stop();
  } 
}

void Channel::sweep_run(){
  
}

void Channel::stop(){
    //Set DAC to 0
    done = true;
    mode = Mode::OFF;
}

void Channel::sweep_steps_config(UserCmd& cmd){

  sweep_.range_in_mV_ = cmd.param.Sweep.range_in_V * 1000; 
  sweep_.step_size_ = cmd.param.Sweep.step_size;
   
  sweep_.single_sweep_steps_ = sweep_.range_in_mV_ / sweep_.step_size_;
}

