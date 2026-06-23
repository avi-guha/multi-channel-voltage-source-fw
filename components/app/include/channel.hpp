#pragma once

#include "ad717x.h"
#include "ad5761r.h"
#include "task_comms.hpp"

constexpr const float R_1K = 1000;
enum class SweepPhase { FIRST, SECOND, THIRD};

class Channel{

  public:
    Channel();
    uint8_t channel_id;
    Mode mode;
    bool done;
    
    bool init(uint8_t id, ad717x_dev* adc_dev, ad5761r_dev* dac_dev);
    void update(UserCmd& cmd); 
    void sweep_run();
    void steady_run();
    void stop();

  private:

    struct SteadyParams{
      bool initialized_ = false;
      uint16_t voltage_ = 0;
      TickType_t duration_in_ticks_ = 0;
      TickType_t finish_time_ = 0;
    };

    struct SweepParams{
      SweepPhase phase_ = SweepPhase::FIRST;
      float range_in_mV_ = 0.0f;
      float step_size_ = 0.0f;
      uint16_t step_count_ = 0;
      uint32_t single_sweep_steps_ = 0;
      uint32_t total_steps_ = 0;
    };

    SteadyParams steady_;
    SweepParams sweep_;

    ad717x_dev* adc_dev_;
    ad5761r_dev* dac_dev_;

    int32_t voltage_read;

    void time_to_xtickcount(UserCmd& cmd); 
    void sweep_steps_config(UserCmd& cmd);

};
