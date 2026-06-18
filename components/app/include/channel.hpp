#pragma once

#include "task_comms.hpp"


enum class SweepPhase { FIRST, SECOND, THIRD};

class Channel{

  public:
    Channel();
    uint8_t channel_id;
    Mode mode;
    bool done;
    
    // bool init(uint8_t channel_id,Ad7172_2* adc, Ad5761* dac);
    void update(UserCmd& cmd); 
    void sweep_run();
    void steady_run();
    void stop();

  private:

    struct SteadyParams{
      bool initialized_ = false;
      int voltage_ = 0;
      TickType_t duration_in_ticks_ = 0;
      TickType_t finish_time_ = 0;
    };

    struct SweepParams{
      SweepPhase phase_ = SweepPhase::FIRST;
      float range_in_mV_ = 0.0f;
      float step_size_ = 0.0f;
      int step_count_ = 0;
      uint32_t single_sweep_steps_ = 0;
      uint32_t total_steps_ = 0;
    };

    SteadyParams steady_;
    SweepParams sweep_;

    // Ad7172_2* adc_;
    // Ad5761* dac_;
    void time_to_xtickcount(UserCmd& cmd); 
    void sweep_steps_config(UserCmd& cmd);
};
