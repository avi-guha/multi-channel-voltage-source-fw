#pragma once
#include "task_comms.hpp"
#include "states/state_idle.hpp"
#include "states/state_error.hpp"
#include "states/state_sweep.hpp"
#include "states/state_steady.hpp"
#include "fsm_types.hpp"


// enum class State : uint8_t { IDLE, SWEEP, STEADY, ERROR };
//
// enum class Event : uint8_t {
//   NO_EVENT, STOP, START_SWEEP, START_STEADY, 
//   DONE_SWEEP, DONE_STEADY, ERROR, CONTINUE
// };
  

class FSM { 
  public:
   FSM(); 
   bool begin();
   void run();

   static void fsm_task(void* arg);

  private:
   State current_state;
   State next_state;
   Event event;
  
   void process_event();
   void state_run();
   void state_exit();
   void state_enter();
};
