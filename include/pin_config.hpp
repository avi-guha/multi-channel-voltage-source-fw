#pragma once

#include <cstdint>

namespace pins {

constexpr int kSpiSclk = 18;
constexpr int kSpiMosi = 23;
constexpr int kSpiMiso = 19;
constexpr int kDacCs = 5;
constexpr int kAdcCs = 4;

}  // namespace pins

namespace app_config {

constexpr uint32_t kDacClockHz = 10'000'000;
constexpr uint32_t kAdcClockHz = 8'000'000;

constexpr float kDacHardwareMinVoltage = -5.0f;
constexpr float kDacHardwareMaxVoltage = 5.0f;
constexpr float kDacTargetMinVoltage = -2.5f;
constexpr float kDacTargetMaxVoltage = 2.5f;

constexpr float kAdcReferenceVoltage = 2.5f;

}  // namespace app_config
