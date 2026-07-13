#pragma once

extern "C" {
  #include "ad717x.h"
  #include "ad5761r.h"
}

#include "task_comms.hpp"

constexpr const float OPAMP_GAIN = 125.6882793f; // G = 1 + 50k / 401 Ohm <- R_G
constexpr const float ADC_VREF = 2.5;
constexpr const float ADC_OFFSET = static_cast<float>(0x800000);
constexpr const float ADC_GAIN = static_cast<float>(0x555555);
constexpr const float DECI_24BIT = static_cast<float>(0x800000);
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
      bool timer_en_ = false;
      float voltage_ = 0.0f;
      TickType_t duration_in_ticks_ = 0;
      TickType_t finish_time_ = 0;
    };

    struct SweepParams{
      SweepPhase phase_ = SweepPhase::FIRST;
      float range_in_V_ = 0.0f;
      float step_size_V_ = 0.0f;
      float voltage_in_V_ = 0.0f;
      uint32_t single_sweep_steps_ = 0;
      uint32_t total_steps_ = 0;
    };

    SteadyParams steady_;
    SweepParams sweep_;

    ad717x_dev* adc_dev_;
    ad5761r_dev* dac_dev_;

    int32_t adc_raw_data;

    void time_to_xtickcount(UserCmd& cmd); 
    void sweep_steps_config(UserCmd& cmd);
    uint16_t voltage_to_bin(float voltage);
    float bin_to_voltage(uint32_t bin);
    float get_current();
};
