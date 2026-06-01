#pragma once

#include "shared_spi_bus.hpp"

class Ad5761 {
 public:
  Ad5761(SharedSpiBus& bus, const SpiDeviceConfig& spi_config);

  bool begin();
  void setDACVoltage(float voltage);

 private:
  static constexpr uint8_t kCmdWriteInput = 0x01;
  static constexpr uint8_t kCmdUpdateDac = 0x02;
  static constexpr uint8_t kCmdWriteAndUpdateDac = 0x03;
  static constexpr uint8_t kCmdWriteControl = 0x04;
  static constexpr uint8_t kCmdSoftwareDataReset = 0x07;
  static constexpr uint8_t kCmdSoftwareFullReset = 0x0F;

  bool writeCommand(uint8_t command, uint16_t data);
  uint16_t encodeVoltage(float voltage) const;

  SharedSpiBus& bus_;
  const SpiDeviceConfig& spi_config_;
};
