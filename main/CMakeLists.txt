idf_component_register(
    SRCS
        "app_main.cpp"
        "../src/ad5761.cpp"
        "../src/ad7172.cpp"
        "../src/demo_app.cpp"
        "../src/shared_spi_bus.cpp"
    INCLUDE_DIRS
        "../include"
    REQUIRES
        esp_driver_gpio
        esp_driver_spi
        esp_timer
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_17)
