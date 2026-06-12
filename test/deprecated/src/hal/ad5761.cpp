#include <esp_log.h>
#include <algorithm>
#include <cmath>

#include "hal/ad5761.hpp"
#include "hal/pin_config.hpp"


static constexpr const char* TAG = "DAC";
constexpr uint16_t kControlRegisterValue = 0x020A;


Ad5761::Ad5761()
    : bus_(nullptr),
      spi_config_(nullptr) {}

bool Ad5761::begin(SharedSpiBus* bus, const SpiDeviceConfig* spi_config) {

  if (bus == nullptr || spi_config == nullptr) return false; 

  bus_ = bus;
  spi_config_ = spi_config;

  if (!writeCommand(kCmdSoftwareFullReset, 0x0000)) {
    ESP_LOGE(TAG, "%s software reset failed", spi_config_->name);
    return false;
  }
  esp_rom_delay_us(100);

  if (!writeCommand(kCmdWriteControl, kControlRegisterValue)) {
    ESP_LOGE(TAG, "%s control register write failed", spi_config_->name);
    return false;
  }

  setDACVoltage(0.0f);
  ESP_LOGI(TAG,
                    "%s initialized with +/-5 V range, command clamp %.1f V to %.1f V",
                    spi_config_ -> name,
                    app_config::kDacTargetMinVoltage,
                    app_config::kDacTargetMaxVoltage);
  return true;
}

void Ad5761::setDACVoltage(float voltage) {
  const uint16_t dac_code = encodeVoltage(voltage);
  if (!writeCommand(kCmdWriteAndUpdateDac, dac_code)) {
    ESP_LOGE(TAG, "failed to update %s to %.4f V", spi_config_->name, voltage);
  }
}

bool Ad5761::writeCommand(uint8_t command, uint16_t data) {
  const uint32_t frame = (static_cast<uint32_t>(command & 0x0F) << 16U) | data;
  const uint8_t tx_buffer[3] = {
      static_cast<uint8_t>((frame >> 16U) & 0xFF),
      static_cast<uint8_t>((frame >> 8U) & 0xFF),
      static_cast<uint8_t>(frame & 0xFF),
  };

  if (!bus_->beginSession(*spi_config_)) {
    return false;
  }

  const bool success = bus_->transfer(tx_buffer, nullptr, sizeof(tx_buffer));
  bus_->endSession();
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
