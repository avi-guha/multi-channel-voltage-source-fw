#pragma once

#include <cstddef>
#include <cstdint>

#if defined(ARDUINO)
#include <SPI.h>
#else
#include "driver/spi_master.h"
#endif

enum class SpiMode : uint8_t {
  kMode0 = 0,
  kMode1 = 1,
  kMode2 = 2,
  kMode3 = 3,
};

struct SpiDeviceConfig {
  const char* name;
  int cs_pin;
  uint32_t clock_hz;
  SpiMode mode;
};

class SharedSpiBus {
 public:
  SharedSpiBus() = default;

  bool begin(int sclk_pin,
             int miso_pin,
             int mosi_pin,
             const SpiDeviceConfig* devices,
             size_t device_count);

  bool beginSession(const SpiDeviceConfig& device);
  bool transfer(const uint8_t* tx_data, uint8_t* rx_data, size_t length_bytes);
  int readMisoLevel() const;
  void endSession();

 private:
#if defined(ARDUINO)
  SPIClass* spi_ = &SPI;
#else
  struct DeviceHandleSlot {
    int cs_pin = -1;
    spi_device_handle_t handle = nullptr;
  };

  static constexpr size_t kMaxDevices = 4;

  spi_host_device_t host_ = SPI2_HOST;
  DeviceHandleSlot device_handles_[kMaxDevices];
  size_t device_handle_count_ = 0;
  spi_device_handle_t current_handle_ = nullptr;
#endif

  const SpiDeviceConfig* current_device_ = nullptr;
  int miso_pin_ = -1;
};
