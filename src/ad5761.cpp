#include "../include/ad5761.hpp"

#include <algorithm>
#include <cmath>

#include "../include/pin_config.hpp"
#include "../include/platform_abstraction.hpp"

namespace {

constexpr const char* kLogTag = "AD5761";
constexpr uint16_t kControlRegisterValue = 0x020A;

}  // namespace

Ad5761::Ad5761(SharedSpiBus& bus, const SpiDeviceConfig& spi_config)
    : bus_(bus), spi_config_(spi_config) {}

bool Ad5761::begin() {
  if (!writeCommand(kCmdSoftwareFullReset, 0x0000)) {
    platform::logError(kLogTag, "software reset failed");
    return false;
  }
  platform::delayUs(100);

  if (!writeCommand(kCmdWriteControl, kControlRegisterValue)) {
    platform::logError(kLogTag, "control register write failed");
    return false;
  }

  setDACVoltage(0.0f);
  platform::logInfo(kLogTag,
                    "initialized with +/-5 V range, command clamp %.1f V to %.1f V",
                    app_config::kDacTargetMinVoltage,
                    app_config::kDacTargetMaxVoltage);
  return true;
}

void Ad5761::setDACVoltage(float voltage) {
  const uint16_t dac_code = encodeVoltage(voltage);
  if (!writeCommand(kCmdWriteAndUpdateDac, dac_code)) {
    platform::logError(kLogTag, "failed to update DAC to %.4f V", voltage);
  }
}

bool Ad5761::writeCommand(uint8_t command, uint16_t data) {
  const uint32_t frame = (static_cast<uint32_t>(command & 0x0F) << 16U) | data;
  const uint8_t tx_buffer[3] = {
      static_cast<uint8_t>((frame >> 16U) & 0xFF),
      static_cast<uint8_t>((frame >> 8U) & 0xFF),
      static_cast<uint8_t>(frame & 0xFF),
  };

  if (!bus_.beginSession(spi_config_)) {
    return false;
  }

  const bool success = bus_.transfer(tx_buffer, nullptr, sizeof(tx_buffer));
  bus_.endSession();
  return success;
}

uint16_t Ad5761::encodeVoltage(float voltage) const {
  const float clamped_voltage =
      std::clamp(voltage, app_config::kDacTargetMinVoltage, app_config::kDacTargetMaxVoltage);
  const float normalized =
      (clamped_voltage - app_config::kDacHardwareMinVoltage) /
      (app_config::kDacHardwareMaxVoltage - app_config::kDacHardwareMinVoltage);
  return static_cast<uint16_t>(std::lround(normalized * 65535.0f));
}
