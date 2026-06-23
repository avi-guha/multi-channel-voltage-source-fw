#pragma once

#include <cstdint>

namespace pins {

constexpr int SPI_SCLK = 18;
constexpr int SPI_MOSI = 23;
constexpr int SPI_MISO = 19;
constexpr int CS_DAC0 = 5;
constexpr int CS_DAC1 = 17;
constexpr int CS_DAC2 = 16;
constexpr int CS_DAC3 = 4;
constexpr int CS_ADC = 21;

}  

namespace app_config {

constexpr uint32_t DAC_FREQ = 10'000'000; //Hz
constexpr uint32_t ADC_FREQ = 8'000'000;  //Hz

constexpr float DAC_REF_MIN_VOLTAGE = -5.0f;    //V
constexpr float DAC_REF_MAX_VOLTAGE = 5.0f;     //V
constexpr float DAC_TARGET_MIN_VOLTAGE = -2.5f; //V
constexpr float DAC_TARGET_MAX_VOLTAGE = 2.5f;  //V

constexpr float ADC_REF_VOLTAGE = 2.5f;   //V

}  
