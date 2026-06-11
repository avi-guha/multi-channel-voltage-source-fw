#include <cstring>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include "shared_spi_bus.hpp"


static constexpr const char* TAG = "SharedSpiBus";

bool SharedSpiBus::begin(int sclk_pin, int miso_pin, int mosi_pin,
                         const SpiDeviceConfig* devices, size_t device_count) {

  miso_pin_ = miso_pin;

  spi_bus_config_t bus_config = {};
  bus_config.sclk_io_num = sclk_pin;
  bus_config.mosi_io_num = mosi_pin;
  bus_config.miso_io_num = miso_pin;
  bus_config.quadwp_io_num = -1;
  bus_config.quadhd_io_num = -1;
  bus_config.max_transfer_sz = 32;

  const esp_err_t init_result = spi_bus_initialize(host_, &bus_config, SPI_DMA_DISABLED);
  if ((init_result != ESP_OK) && (init_result != ESP_ERR_INVALID_STATE)) {
    ESP_LOGE(TAG, "spi_bus_initialize failed: %d", static_cast<int>(init_result));
    return false;
  }

  if (device_count > kMaxDevices) {
    ESP_LOGE(TAG, "too many SPI devices: %u", static_cast<unsigned>(device_count));
    return false;
  }

  device_handle_count_ = 0;
  for (size_t i = 0; i < device_count; ++i) {
    gpio_config_t cs_config = {};
    cs_config.pin_bit_mask = 1ULL << devices[i].cs_pin;
    cs_config.mode = GPIO_MODE_OUTPUT;
    cs_config.pull_up_en = GPIO_PULLUP_DISABLE;
    cs_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cs_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&cs_config);
    gpio_set_level(static_cast<gpio_num_t>(devices[i].cs_pin), 1);

    spi_device_interface_config_t dev_config = {};
    dev_config.clock_speed_hz = static_cast<int>(devices[i].clock_hz);
    dev_config.mode = static_cast<uint8_t>(devices[i].mode);
    dev_config.spics_io_num = -1;
    dev_config.queue_size = 1;

    spi_device_handle_t handle = nullptr;
    const esp_err_t add_result = spi_bus_add_device(host_, &dev_config, &handle);
    if ((add_result != ESP_OK) && (add_result != ESP_ERR_INVALID_STATE)) {
      ESP_LOGE(TAG,
                         "spi_bus_add_device failed for %s: %d",
                         devices[i].name,
                         static_cast<int>(add_result));
      return false;
    }

    if (handle == nullptr) {
      ESP_LOGE(TAG, "missing device handle for %s", devices[i].name);
      return false;
    }

    device_handles_[device_handle_count_].cs_pin = devices[i].cs_pin;
    device_handles_[device_handle_count_].handle = handle;
    ++device_handle_count_;
  }

  return true;
}

bool SharedSpiBus::beginSession(const SpiDeviceConfig& device) {
  if (current_device_ != nullptr) {
    ESP_LOGE(TAG, "SPI session already active");
    return false;
  }

  current_handle_ = nullptr;
  for (size_t i = 0; i < device_handle_count_; ++i) {
    if (device_handles_[i].cs_pin == device.cs_pin) {
      current_handle_ = device_handles_[i].handle;
      break;
    }
  }

  if (current_handle_ == nullptr) {
    ESP_LOGE(TAG, "no SPI handle registered for %s", device.name);
    return false;
  }

  gpio_set_level(static_cast<gpio_num_t>(device.cs_pin), 0);

  current_device_ = &device;
  return true;
}

bool SharedSpiBus::transfer(const uint8_t* tx_data, uint8_t* rx_data, size_t length_bytes) {
  if ((current_device_ == nullptr) || (length_bytes == 0U)) {
    return false;
  }

  uint8_t dummy_tx[32];
  if ((tx_data == nullptr) && (length_bytes > sizeof(dummy_tx))) {
    ESP_LOGE(TAG, "dummy transfer too large: %u", static_cast<unsigned>(length_bytes));
    return false;
  }

  if (tx_data == nullptr) {
    std::memset(dummy_tx, 0xFF, length_bytes);
  }

  spi_transaction_t transaction = {};
  transaction.length = length_bytes * 8U;
  transaction.rxlength = (rx_data != nullptr) ? (length_bytes * 8U) : 0U;
  transaction.tx_buffer = (tx_data != nullptr) ? tx_data : dummy_tx;
  transaction.rx_buffer = rx_data;

  const esp_err_t result = spi_device_polling_transmit(current_handle_, &transaction);
  if (result != ESP_OK) {
    ESP_LOGE(TAG,
                       "polling transmit failed for %s: %d",
                       current_device_->name,
                       static_cast<int>(result));
    return false;
  }
  return true;
}

int SharedSpiBus::readMisoLevel() const {
  return gpio_get_level(static_cast<gpio_num_t>(miso_pin_));
}

void SharedSpiBus::endSession() {
  if (current_device_ == nullptr) {
    return;
  }

  gpio_set_level(static_cast<gpio_num_t>(current_device_->cs_pin), 1);
  current_handle_ = nullptr;
  current_device_ = nullptr;
}
