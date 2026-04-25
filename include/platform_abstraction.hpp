#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdio>

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

namespace platform {

inline void initConsole() {
#if defined(ARDUINO)
  Serial.begin(115200);
  const uint32_t start = ::millis();
  while (!Serial && (::millis() - start) < 3000U) {
    ::delay(10);
  }
#endif
}

inline void delayMs(uint32_t delay_ms) {
#if defined(ARDUINO)
  ::delay(delay_ms);
#else
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
#endif
}

inline void delayUs(uint32_t delay_us) {
#if defined(ARDUINO)
  ::delayMicroseconds(delay_us);
#else
  esp_rom_delay_us(delay_us);
#endif
}

inline uint32_t millis32() {
#if defined(ARDUINO)
  return ::millis();
#else
  return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
#endif
}

inline void logImpl(const char* level, const char* tag, const char* fmt, va_list args) {
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), fmt, args);
#if defined(ARDUINO)
  Serial.printf("[%s] %s: %s\n", level, tag, buffer);
#else
  std::printf("[%s] %s: %s\n", level, tag, buffer);
#endif
}

inline void logInfo(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  logImpl("I", tag, fmt, args);
  va_end(args);
}

inline void logError(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  logImpl("E", tag, fmt, args);
  va_end(args);
}

}  // namespace platform
