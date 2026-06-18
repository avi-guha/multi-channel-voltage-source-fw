idf_component_register(
  SRCS  "dac.cpp" "shared_spi_bus.cpp" 
  INCLUDE_DIRS "include"
  REQUIRES adc
           dac
           esp32_spi
           esp_driver_spi
           esp_driver_gpio
           esp_timer
)
