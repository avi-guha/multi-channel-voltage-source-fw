#pragma once
#include "task_comms.hpp"
#include "ad5761.hpp"


enum class SweepPhase { FIRST, SECOND, THIRD};

class Channel{

  public:
    Channel();

    Mode mode;
    bool done;

    void update(UserCmd& cmd); 
    void sweep_run();
    void steady_run();
    void stop();

  private:

    struct SteadyParams{
      bool initialized_;
      float voltage_;
      float duration_in_ticks_;
      float finish_time_;
    };

    struct SweepParams{
      SweepPhase phase_;
      float range_in_mV_;
      float step_size_;
      int step_count_;
      uint32_t single_sweep_steps_;
      uint32_t total_steps_;
    };

    SteadyParams steady_;
    SweepParams sweep_;

    Ad5761 dac();
    void time_to_xtickcount(UserCmd& cmd); 
    void sweep_steps_config(UserCmd& cmd);
};
