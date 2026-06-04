#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ad5761.hpp"
#include "ad7172.hpp"

enum class Mode : uint8_t { SWEEP, STEADY };
enum class TimeUnit : uint8_t { Sec, Min, Hour };

struct SteadyParams{
  float voltage;
};

struct SweepParams{
  float sweep_range;
  float step;
};

struct UserCmd{
  Mode mode;
  float duration;
  TimeUnit unit;
  union {
    struct SteadyParams SteadyMode;
    struct SweepParams SweepMode;
  } params;
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
