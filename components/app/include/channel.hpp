/**
 * @file channel.hpp
 * @brief Per-channel state machine controlling one DAC output and its ADC readback.
 * @details Each Channel owns one AD5761R DAC and shares the AD7172-2 ADC.
 *          The coordinator dispatches mode changes via update() and drives
 *          the per-iteration work with steady_run() / sweep_run(). Sample
 *          records are pushed to data_queue for the log task.
 */

#pragma once

extern "C" {
  #include "ad717x.h"
  #include "ad5761r.h"
}

#include "task_comms.hpp"

/** @brief Transimpedance stage gain: G = 1 + 50k / 401Ω. */
constexpr const float OPAMP_GAIN = 125.4586051f;
/** @brief ADC internal reference voltage (V). */
constexpr const float ADC_VREF = 2.5;
/** @brief Offset-binary midscale for 24-bit ADC data. */
constexpr const float ADC_OFFSET = static_cast<float>(0x800000);
/** @brief Full-scale calibration gain constant used by AD717x conversion math. */
constexpr const float ADC_GAIN = static_cast<float>(0x555555);
/** @brief 2^23, used as denominator in voltage conversion. */
constexpr const float DECI_24BIT = static_cast<float>(0x800000);
/** @brief Current-sense resistor value in ohms. */
constexpr const float R_1K = 996.675;

/**
 * @brief Phases of a triangular voltage sweep.
 * @details FIRST: 0 → +range. SECOND: +range → −range. THIRD: −range → 0.
 */
enum class SweepPhase { FIRST, SECOND, THIRD};

/**
 * @brief One output channel: DAC voltage set + ADC current readback.
 */
class Channel{

  public:

    Channel();
    uint8_t channel_id;   
    Mode mode;            
    bool done;            

    /**
     * @brief Bind hardware handles and drive DAC to 0 V.
     * @param id Channel index (0 .. NUM_CHANNELS-1).
     * @param adc_dev Shared AD7172-2 device handle.
     * @param dac_dev This channel's AD5761R device handle.
     * @return true on success, false if either handle is null.
     */
    bool init(uint8_t id, ad717x_dev* adc_dev, ad5761r_dev* dac_dev);

    /**
     * @brief Apply a new UserCmd — switches mode and resets per-mode state.
     * @param cmd Parsed user command targeting this channel.
     */
    void update(UserCmd& cmd);

    /** @brief One sweep step: set next DAC voltage, sample current, advance phase. */
    void sweep_run();

    /** @brief One steady iteration: initialize DAC on first call, then sample current. */
    void steady_run();

    /** @brief Drop DAC to 0 V, mark channel OFF, emit a final DataLog record. */
    void stop();

  private:

    struct SteadyParams{
      bool initialized_ = false;
      bool timer_en_ = false;
      float voltage_ = 0.0f;
      int64_t duration_us_ = 0;
      int64_t finish_time_ = 0;
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

    /** @brief Convert cmd's duration+time_unit into FreeRTOS ticks; set steady_.timer_en_. */
    void time_to_us(UserCmd& cmd);

    /**
     * @brief Convert a voltage (V, ±5V range) to an AD5761R 16-bit code.
     * @param voltage Desired DAC output in volts.
     * @return 16-bit DAC input code.
     */
    uint16_t voltage_to_bin(float voltage);

    /**
     * @brief Convert a 24-bit AD7172-2 sample to volts at the ADC input pin.
     * @param bin Offset-binary 24-bit code from the ADC.
     * @return Voltage in volts (post-PGA, pre-opamp gain).
     */
    float bin_to_voltage(uint32_t bin);

    /**
     * @brief Poll ADC until a fresh sample for THIS channel is available; return current in µA.
     * @return Measured current in microamps (V_adc / (R × opamp_gain), scaled to µA).
     */
    float get_current();
};
