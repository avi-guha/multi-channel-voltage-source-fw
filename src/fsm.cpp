#include <esp_log.h>
#include "fsm.hpp"
#include "task_comms.hpp"
#include "states/state_idle.hpp"
#include "states/state_error.hpp"
#include "states/state_sweep.hpp"
#include "states/state_steady.hpp"

static const char* TAG = "FSM";

static FSM fsm;


FSM::FSM(): current_state(State::IDLE), next_state(State::IDLE), event(Event::NO_EVENT){}

void FSM::fsm_task(void* arg){
  if(!fsm.begin()){
    ESP_LOGE(TAG,"Failed to start.");
  }
  else{
    fsm.run();
  }
}

bool FSM::begin() {
  return true; 
}

void FSM::run() {
  
  while(true){
    if (event_queue != 0){
      //Fall back for no event queue return required?

      xQueueReceive(event_queue, &event, pdMS_TO_TICKS(10));
      process_event();

      if (next_state != current_state){
        state_exit();
        current_state = next_state;
        state_enter();
      } 

      state_run();
    }
  }
}


void FSM::state_exit(){

  switch (current_state) {

    case State::IDLE:
      idle_exit();
      break;
    case State::SWEEP:
      sweep_exit();
      break;
    case State::STEADY:
      steady_exit();
      break;
    case State::ERROR:
      error_exit();
      break;
  }
}


void FSM::state_enter(){

  switch (current_state) {

    case State::IDLE:
      idle_enter();
      break;
    case State::SWEEP:
      sweep_enter();
      break;
    case State::STEADY:
      steady_enter();
      break;
    case State::ERROR:
      error_enter();
      break;
  }
}


void FSM::state_run() {
  
  switch (current_state){

    case State::IDLE:
      idle_run();
      break;
    case State::SWEEP:
      sweep_run();
      break;
    case State::STEADY:
      steady_run();
      break;
    case State::ERROR:
      error_run();
      break;
  } 
}


void FSM::process_event(){
  
  switch (current_state) {

    case State::IDLE:
      if (event == Event::START_STEADY) {
        next_state = State::STEADY;
      }
      else if (event == Event::START_SWEEP) {
        next_state = State::SWEEP;
      }
      break;

    case State::SWEEP:
      if (event == Event::DONE_STEADY) {
        next_state = State::IDLE;
      }
      else if (event == Event::ERROR) {
        next_state = State::ERROR;
      }
      break;

    case State::STEADY:
      if (event == Event::DONE_SWEEP) {
        next_state = State::IDLE;
      }
      else if (event == Event::ERROR) {
        next_state = State::ERROR;
      }
      break;
      
    case State::ERROR:
      if (event == Event::CONTINUE) {
        next_state = State::IDLE;
      }
      break;
    
    default:
      next_state = current_state;
  };
} 
