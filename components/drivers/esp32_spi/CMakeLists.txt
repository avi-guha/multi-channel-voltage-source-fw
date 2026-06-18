idf_component_register(
  SRCS "no_os_esp32_spi.c"
  INCLUDE_DIRS "include"
  REQUIRES no_os_include
           adc
           dac
           esp_driver_spi
           esp_driver_gpio
           esp_timer
)
