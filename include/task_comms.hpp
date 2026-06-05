#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


enum class Mode : uint8_t { SWEEP, STEADY, OFF};
enum class TimeUnit : uint8_t { Sec, Min, Hour };

struct SteadyParams{
  float voltage;
};

struct SweepParams{
  float range_in_V;
  float step_size;
};

struct UserCmd{
  uint8_t channel_id;
  Mode mode;
  float duration;
  TimeUnit time_unit;
  union {
    struct SteadyParams Steady;
    struct SweepParams Sweep;
  } param;
  bool valid;
};


struct DataLog{
  float voltage;
  float current;
  float time;
};


extern QueueHandle_t data_queue;
extern QueueHandle_t user_cmd_queue;
extern QueueHandle_t event_queue;

void user_cmd_task(void* arg);
