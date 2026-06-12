#include <esp_log.h>
#include <cmath>
#include <limits>

#include "hal/ad7172.hpp"
#include "hal/pin_config.hpp"
#include "hal/platform_abstraction.hpp"

static constexpr const char* TAG = "ADC";
constexpr uint8_t kSpiReadBit = 0x40;
constexpr uint8_t kAin4 = 4;


Ad7172_2::Ad7172_2(SharedSpiBus& bus, const SpiDeviceConfig& spi_config)
    : bus_(bus), spi_config_(spi_config) {}

bool Ad7172_2::begin() {
  if (!reset()) {
    ESP_LOGE(TAG, "ADC reset failed");
    return false;
  }
  platform::delayMs(2);

  uint32_t id_value = 0;
  if (!readRegister(kRegId, id_value, 2)) {
    ESP_LOGE(TAG, "unable to read ID register");
    return false;
  }

  if (!writeRegister(kRegIfMode, kIfModeDataStatus, 2)) {
    return false;
  }
  if (!writeRegister(kRegAdcMode, kAdcModeInternalRefContinuous, 2)) {
    return false;
  }
  if (!writeRegister(kRegSetupCon0, kSetupCon0BipolarInternalRef, 2)) {
    return false;
  }
  if (!writeRegister(kRegFiltCon0, kFiltCon0, 2)) {
    return false;
  }

  for (uint8_t channel = 0; channel < 4; ++channel) {
    const uint16_t channel_register = makeChannelRegister(channel, kAin4);
    if (!writeRegister(static_cast<uint8_t>(kRegCh0 + channel), channel_register, 2)) {
      ESP_LOGE(TAG, "failed to configure channel %u", static_cast<unsigned>(channel));
      return false;
    }
  }

  ESP_LOGI(TAG, "initialized, ID=0x%04X", static_cast<unsigned>(id_value));
  return true;
}

float Ad7172_2::readADCChannel(uint8_t channel) {
  if (channel > 3U) {
    ESP_LOGE(TAG, "invalid channel index %u", static_cast<unsigned>(channel));
    return std::numeric_limits<float>::quiet_NaN();
  }

  const uint32_t start = platform::millis32();
  while ((platform::millis32() - start) < 250U) {
    if (!bus_.beginSession(spi_config_)) {
      return std::numeric_limits<float>::quiet_NaN();
    }

    const bool ready = waitForReadyInSession(250U);
    if (!ready) {
      bus_.endSession();
      break;
    }

    uint32_t raw_code = 0;
    uint8_t status = 0;
    const bool read_ok = readDataRegisterInSession(raw_code, status);
    bus_.endSession();
    if (!read_ok) {
      return std::numeric_limits<float>::quiet_NaN();
    }

    const uint8_t sampled_channel = static_cast<uint8_t>(status & 0x03U);
    const float voltage = codeToVoltage(raw_code);
    latest_channel_voltages_[sampled_channel] = voltage;
    has_channel_sample_[sampled_channel] = true;

    if ((status & 0x40U) != 0U) {
      ESP_LOGE(TAG,
                         "ADC error flag set on channel %u, raw=0x%06X",
                         static_cast<unsigned>(sampled_channel),
                         static_cast<unsigned>(raw_code));
    }

    if (sampled_channel == channel) {
      return voltage;
    }
  }

  if (has_channel_sample_[channel]) {
    return latest_channel_voltages_[channel];
  }

  ESP_LOGE(TAG, "timed out waiting for channel %u", static_cast<unsigned>(channel));
  return std::numeric_limits<float>::quiet_NaN();
}

bool Ad7172_2::reset() {
  const uint8_t reset_bytes[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (!bus_.beginSession(spi_config_)) {
    return false;
  }
  const bool success = bus_.transfer(reset_bytes, nullptr, sizeof(reset_bytes));
  bus_.endSession();
  return success;
}

bool Ad7172_2::writeRegister(uint8_t reg, uint32_t value, size_t value_length_bytes) {
  uint8_t tx_buffer[4] = {};
  if ((value_length_bytes == 0U) || (value_length_bytes > 3U)) {
    return false;
  }

  tx_buffer[0] = reg & 0x3F;
  for (size_t i = 0; i < value_length_bytes; ++i) {
    const size_t shift = (value_length_bytes - 1U - i) * 8U;
    tx_buffer[1U + i] = static_cast<uint8_t>((value >> shift) & 0xFFU);
  }

  if (!bus_.beginSession(spi_config_)) {
    return false;
  }
  const bool success = bus_.transfer(tx_buffer, nullptr, value_length_bytes + 1U);
  bus_.endSession();
  return success;
}

bool Ad7172_2::readRegister(uint8_t reg, uint32_t& value, size_t value_length_bytes) {
  uint8_t tx_buffer[4] = {};
  uint8_t rx_buffer[4] = {};
  if ((value_length_bytes == 0U) || (value_length_bytes > 3U)) {
    return false;
  }

  tx_buffer[0] = static_cast<uint8_t>(kSpiReadBit | (reg & 0x3F));
  for (size_t i = 1; i < (value_length_bytes + 1U); ++i) {
    tx_buffer[i] = 0xFF;
  }

  if (!bus_.beginSession(spi_config_)) {
    return false;
  }
  const bool success = bus_.transfer(tx_buffer, rx_buffer, value_length_bytes + 1U);
  bus_.endSession();
  if (!success) {
    return false;
  }

  value = 0;
  for (size_t i = 0; i < value_length_bytes; ++i) {
    value = (value << 8U) | rx_buffer[1U + i];
  }
  return true;
}

bool Ad7172_2::waitForReadyInSession(uint32_t timeout_ms) {
  const uint32_t start = platform::millis32();
  while (bus_.readMisoLevel() != 0) {
    if ((platform::millis32() - start) >= timeout_ms) {
      return false;
    }
    platform::delayUs(50);
  }
  return true;
}

bool Ad7172_2::readDataRegisterInSession(uint32_t& raw_code, uint8_t& status) {
  const uint8_t tx_buffer[5] = {
      static_cast<uint8_t>(kSpiReadBit | (kRegData & 0x3F)),
      0xFF,
      0xFF,
      0xFF,
      0xFF,
  };
  uint8_t rx_buffer[5] = {};

  if (!bus_.transfer(tx_buffer, rx_buffer, sizeof(tx_buffer))) {
    return false;
  }

  raw_code = (static_cast<uint32_t>(rx_buffer[1]) << 16U) |
             (static_cast<uint32_t>(rx_buffer[2]) << 8U) |
             static_cast<uint32_t>(rx_buffer[3]);
  status = rx_buffer[4];
  return true;
}

float Ad7172_2::codeToVoltage(uint32_t raw_code) const {
  const float normalized = (static_cast<float>(raw_code) / 8388608.0f) - 1.0f;
  return normalized * app_config::kAdcReferenceVoltage;
}

uint16_t Ad7172_2::makeChannelRegister(uint8_t positive_input, uint8_t negative_input) const {
  return static_cast<uint16_t>(0x8000U |
                               ((positive_input & 0x1FU) << 5U) |
                               (negative_input & 0x1FU));
}
