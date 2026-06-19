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

  if (param == NULL) return -1;

  struct no_os_spi_desc *d = no_os_calloc(1, sizeof(*d));
  spi_config_extra_t *esp32_spi_config = (spi_config_extra_t *)param->extra; 

  const esp_err_t init_result = 
    spi_bus_initialize(esp32_spi_config->host, &esp32_spi_config->bus, SPI_DMA_DISABLED);

  if ((init_result != ESP_OK) && (init_result != ESP_ERR_INVALID_STATE)) {
    ESP_LOGE(TAG, "spi_bus_initialize failed: %d", (int)init_result);
    return -1;
  }

  *desc = d;
  return 0;
}

int32_t esp32_spi_write_read(struct no_os_spi_desc *desc, uint8_t *count, uint16_t count2){

  return 0;
}

int32_t esp32_spi_transfer(struct no_os_spi_desc *desc, struct no_os_spi_msg *msg, uint32_t length){

  return 0;
}

int32_t esp32_spi_remove(struct no_os_spi_desc *desc){

  return 0;
}

int32_t esp32_spi_transfer_abort(struct no_os_spi_desc *desc){

  return 0;
}

