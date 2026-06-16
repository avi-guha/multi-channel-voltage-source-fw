#include <esp_log.h>
#include <esp_err.h>
#include <no_os_spi.h>
#include <no_os_alloc.h>
#include <driver/gpio.h>
#include "include/spi_bus.h"


int32_t esp32_spi_init(struct no_os_spi_desc **desc, const struct no_os_spi_init_param *param){

  struct no_os_spi_desc *d = no_os_calloc(1, sizeof(*d));

  // const esp_err_t init = spi_bus_initialize(); 


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

