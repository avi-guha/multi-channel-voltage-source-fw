#pragma once

#include "ad5761.hpp"
#include "ad7172.hpp"
#include "pin_config.hpp"
#include "shared_spi_bus.hpp"

enum class State { IDLE, SWEEP, STEADY, ERROR };
enum class Event {
  NO_EVENT, STOP, START_SWEEP, START_STEADY, 
  DONE_SWEEP, DONE_STEADY, ERROR, CONTINUE
};

class FSM { 
  public:
   FSM(); 
   bool begin();


  private:
   State current_state;
   State next_state;
   Event event;
  
   void fsmTask();
   void process_event();
   void state_enter();
   void state_exit();
   void state_run();
};
