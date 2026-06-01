#pragma once

#include <cstdint>

#include "shared_spi_bus.hpp"

class Ad7172_2 {
 public:
  Ad7172_2(SharedSpiBus& bus, const SpiDeviceConfig& spi_config);

  bool begin();
  float readADCChannel(uint8_t channel);

 private:
  static constexpr uint8_t kRegStatus = 0x00;
  static constexpr uint8_t kRegAdcMode = 0x01;
  static constexpr uint8_t kRegIfMode = 0x02;
  static constexpr uint8_t kRegData = 0x04;
  static constexpr uint8_t kRegId = 0x07;
  static constexpr uint8_t kRegCh0 = 0x10;
  static constexpr uint8_t kRegSetupCon0 = 0x20;
  static constexpr uint8_t kRegFiltCon0 = 0x28;

  static constexpr uint8_t kIfModeDataStatus = 0x40;
  static constexpr uint16_t kAdcModeInternalRefContinuous = 0x8000;
  static constexpr uint16_t kSetupCon0BipolarInternalRef = 0x1020;
  static constexpr uint16_t kFiltCon0 = 0x050E;

  bool reset();
  bool writeRegister(uint8_t reg, uint32_t value, size_t value_length_bytes);
  bool readRegister(uint8_t reg, uint32_t& value, size_t value_length_bytes);
  bool waitForReadyInSession(uint32_t timeout_ms);
  bool readDataRegisterInSession(uint32_t& raw_code, uint8_t& status);
  float codeToVoltage(uint32_t raw_code) const;
  uint16_t makeChannelRegister(uint8_t positive_input, uint8_t negative_input) const;

  SharedSpiBus& bus_;
  const SpiDeviceConfig& spi_config_;
  float latest_channel_voltages_[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  bool has_channel_sample_[4] = {false, false, false, false};
};
