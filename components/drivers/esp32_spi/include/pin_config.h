#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

#include <stdint.h>

const uint8_t SPI_SCLK = 18;
const uint8_t SPI_MOSI = 23;
const uint8_t SPI_MISO = 19;
const uint8_t CS_DAC0 = 5;
const uint8_t CS_DAC1 = 17;
const uint8_t CS_DAC2 = 16;
const uint8_t CS_DAC3 = 4;
const uint8_t CS_ADC = 21;


const uint32_t DAC_FREQ = 10000000; //10MHz
const uint32_t ADC_FREQ = 8000000;  //8MHz

const float DAC_REF_MIN_VOLTAGE = -5.0f;    //V
const float DAC_REF_MAX_VOLTAGE = 5.0f;     //V
const float DAC_TARGET_MIN_VOLTAGE = -2.5f; //V
const float DAC_TARGET_MAX_VOLTAGE = 2.5f;  //V

const float ADC_REF_VOLTAGE = 2.5f;   //V

#endif


