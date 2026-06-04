#include "states/state_idle.hpp"
#include "task_comms.hpp"
#include "fsm_types.hpp"

Event event;
struct UserCmd cmd;


void idle_enter(){
  event = Event::NO_EVENT;
}


void idle_run(){

  if (user_cmd_queue != 0){

    xQueuePeek(user_cmd_queue, &cmd, pdMS_TO_TICKS(100));

    switch (cmd.mode){

      case Mode::SWEEP:
        event = Event::START_SWEEP;
        break;

      case Mode::STEADY:
        event = Event::START_STEADY;
        break;

      default:
        event = Event::NO_EVENT;
        break;
    }

    xQueueSend(event_queue, &event, pdMS_TO_TICKS(100)); 
  }
}


void idle_exit(){
  
}
