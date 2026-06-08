#include "deprecated/demo_app.hpp"

#include <cmath>

#include "deprecated/platform_abstraction.hpp"

namespace {

constexpr const char* kLogTag = "DemoApp";
constexpr float kSweepVoltages[] = {-2.5f, -1.25f, 0.0f, 1.25f, 2.5f};
constexpr size_t kSweepVoltageCount = sizeof(kSweepVoltages) / sizeof(kSweepVoltages[0]);

}  // namespace

DemoApp::DemoApp()
    : dac1_(spi_bus_, kDac1SpiConfig),
      dac2_(spi_bus_, kDac2SpiConfig),
      dac3_(spi_bus_, kDac3SpiConfig),
      dac4_(spi_bus_, kDac4SpiConfig),
      adc_(spi_bus_, kAdcSpiConfig) {}

bool DemoApp::begin() {
  platform::initConsole();
  platform::delayMs(200);

  const SpiDeviceConfig devices[] = {
      kDac1SpiConfig,
      kDac2SpiConfig,
      kDac3SpiConfig,
      kDac4SpiConfig,
      kAdcSpiConfig,
  };
  if (!spi_bus_.begin(pins::kSpiSclk,
                      pins::kSpiMiso,
                      pins::kSpiMosi,
                      devices,
                      sizeof(devices) / sizeof(devices[0]))) {
    platform::logError(kLogTag, "shared SPI bus init failed");
    return false;
  }

  if (!beginDac(dac1_, kDac1SpiConfig) || !beginDac(dac2_, kDac2SpiConfig) ||
      !beginDac(dac3_, kDac3SpiConfig) || !beginDac(dac4_, kDac4SpiConfig)) {
    return false;
  }

  if (!adc_.begin()) {
    platform::logError(kLogTag, "%s init failed", kAdcSpiConfig.name);
    return false;
  }

  platform::logInfo(kLogTag, "startup complete: 4 DACs + 1 ADC");
  return true;
}

void DemoApp::runOnce() {
  const float target_voltage = kSweepVoltages[sweep_index_];
  sweep_index_ = (sweep_index_ + 1U) % kSweepVoltageCount;

  setAllDacVoltages(target_voltage);
  platform::delayMs(50);

  platform::logInfo(kLogTag, "DAC1-4 target: %.3f V", target_voltage);
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

bool DemoApp::beginDac(Ad5761& dac, const SpiDeviceConfig& config) {
  if (!dac.begin()) {
    platform::logError(kLogTag, "%s init failed", config.name);
    return false;
  }

  return true;
}

void DemoApp::setAllDacVoltages(float voltage) {
  dac1_.setDACVoltage(voltage);
  dac2_.setDACVoltage(voltage);
  dac3_.setDACVoltage(voltage);
  dac4_.setDACVoltage(voltage);
}
