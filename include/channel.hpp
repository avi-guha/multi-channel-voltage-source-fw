#pragma once
#include "task_comms.hpp"
#include "ad5761.hpp"

class Channel{

  public:

  private:
    uint8_t channel_id_;
    Mode mode_;
    float duration_;
    TimeUnit unit_;
    union {
      struct SteadyParams SteadyMode;
      struct SweepParams SweepMode;
    } params;

    Ad5761 dac_();
};
