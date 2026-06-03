#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

enum class Mode : uint8_t { SWEEP, STEADY };
enum class TimeUnit : uint8_t { Sec, Min, Hour };

extern QueueHandle_t event_queue;

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


