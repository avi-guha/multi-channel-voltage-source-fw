#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


enum class Mode : uint8_t { SWEEP, STEADY, OFF};
enum class TimeUnit : uint8_t { Sec, Min, Hour };


struct SteadyParams{
  float voltage;
  float duration;
  TimeUnit time_unit;
};


struct SweepParams{
  float range_in_V;
  float step_size;
};


struct UserCmd{
  uint8_t channel_id;
  Mode mode;
  union {
    struct SteadyParams Steady;
    struct SweepParams Sweep;
  } param;
};


struct DataLog{
  uint8_t channel_id;
  Mode mode;
  int voltage;
  float current;
  uint32_t time;
};


extern QueueHandle_t data_queue;
extern QueueHandle_t user_cmd_queue;
extern QueueHandle_t event_queue;

void log_task (void* arg);
void user_cmd_task(void* arg);
