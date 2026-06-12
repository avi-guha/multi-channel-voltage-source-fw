idf_component_register(
  SRCS "adc.cpp" "dac.cpp" "ad5761.cpp" "ad7172.cpp" "shared_spi_bus.cpp"
  INCLUDE_DIRS "include"
  REQUIRES adc
           dac
           esp_driver_spi
           esp_driver_gpio
           esp_timer
           
)
