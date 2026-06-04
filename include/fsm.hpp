#pragma once
#include "fsm_types.hpp"


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
