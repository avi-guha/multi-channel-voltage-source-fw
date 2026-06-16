idf_component_register(
  SRCS "spi_bus.c"
  INCLUDE_DIRS "include"
  REQUIRES no_os_include
           esp_driver_spi
           esp_driver_gpio
           esp_timer
)
