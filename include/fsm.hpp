#pragma once

#include "ad5761.hpp"
#include "ad7172.hpp"
#include "pin_config.hpp"
#include "shared_spi_bus.hpp"


class FSM { 
  public:
   FSM(); 
   bool begin();
   void fsmTask();

};
