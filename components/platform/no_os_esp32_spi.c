#include <esp_log.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "spi_configs.h"
#include "no_os_esp32_spi.h"

#define TAG "ESP32-SPI"

int32_t esp32_spi_init(struct no_os_spi_desc **desc, const struct no_os_spi_init_param *param){

  if (!param) {
    ESP_LOGE(TAG, "null param");
    return -1;
  } 

  struct no_os_spi_desc *d = no_os_calloc(1, sizeof(*d));
  spi_device_handle_t *handle = no_os_calloc(1, sizeof(*handle));

  if (!d || !handle) {
    ESP_LOGE(TAG, "Not enough memory");
    return -1;
  }

  esp32_spi_config_t *esp32_spi_config = (esp32_spi_config_t *)param->extra; 
  spi_device_interface_config_t dev_config = {
    .clock_speed_hz = param->max_speed_hz,
    .spics_io_num = param->chip_select,
    .mode = param->mode,
    .queue_size = 1
  };
  
  if (spi_bus_add_device(esp32_spi_config->host, &dev_config, handle) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to add SPI device");
    return -1;
  }

  d->extra = handle;
  *desc = d;
  return 0;
}

int32_t esp32_spi_write_read(struct no_os_spi_desc *desc, uint8_t *data, uint16_t len){

  if (!desc || !data || len == 0) return -1;

  spi_device_handle_t *handle = (spi_device_handle_t *)desc->extra;
  spi_transaction_t transaction = {
    .length = 8 * len,
    .tx_buffer = data,
    .rx_buffer = data
  };

  return spi_device_transmit(*handle, &transaction) == ESP_OK ? 0 : -1;  
}


int32_t esp32_spi_remove(struct no_os_spi_desc *desc){
  if (!desc) return -1;

  spi_device_handle_t *handle = (spi_device_handle_t *)desc->extra;
  spi_bus_remove_device(*handle);
  no_os_free(handle);
  no_os_free(desc);

  return 0;
}


