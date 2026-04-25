#include "../include/demo_app.hpp"

#include <cmath>

#include "../include/platform_abstraction.hpp"

namespace {

constexpr const char* kLogTag = "DemoApp";
constexpr float kSweepVoltages[] = {-2.5f, -1.25f, 0.0f, 1.25f, 2.5f};

}  // namespace

DemoApp::DemoApp() : dac_(spi_bus_, kDacSpiConfig), adc_(spi_bus_, kAdcSpiConfig) {}

bool DemoApp::begin() {
  platform::initConsole();
  platform::delayMs(200);

  const SpiDeviceConfig devices[] = {kDacSpiConfig, kAdcSpiConfig};
  if (!spi_bus_.begin(pins::kSpiSclk, pins::kSpiMiso, pins::kSpiMosi, devices, 2)) {
    platform::logError(kLogTag, "shared SPI bus init failed");
    return false;
  }

  if (!dac_.begin()) {
    platform::logError(kLogTag, "DAC init failed");
    return false;
  }

  if (!adc_.begin()) {
    platform::logError(kLogTag, "ADC init failed");
    return false;
  }

  platform::logInfo(kLogTag, "startup complete");
  return true;
}

void DemoApp::runOnce() {
  const float target_voltage = kSweepVoltages[sweep_index_];
  sweep_index_ = (sweep_index_ + 1U) % (sizeof(kSweepVoltages) / sizeof(kSweepVoltages[0]));

  dac_.setDACVoltage(target_voltage);
  platform::delayMs(50);

  platform::logInfo(kLogTag, "DAC target: %.3f V", target_voltage);
  for (uint8_t channel = 0; channel < 4U; ++channel) {
    const float measured_voltage = adc_.readADCChannel(channel);
    if (std::isnan(measured_voltage)) {
      platform::logError(kLogTag, "AIN%u -> read failed", static_cast<unsigned>(channel));
      continue;
    }

    platform::logInfo(
        kLogTag, "AIN%u referenced to AIN4: %.6f V", static_cast<unsigned>(channel), measured_voltage);
  }

  platform::delayMs(1000);
}
