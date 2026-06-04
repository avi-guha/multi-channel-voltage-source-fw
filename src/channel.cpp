#include "channel.hpp"



uint8_t channel_id_;
Mode mode_;
float duration_;
TimeUnit unit_;
union {
  struct SteadyParams SteadyMode;
  struct SweepParams SweepMode;
} params;

Ad5761 dac_();
Ad7172_2 adc_();
