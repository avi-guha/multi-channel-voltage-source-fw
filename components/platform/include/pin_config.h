#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

#include <stdint.h>
#include <driver/gpio.h>

#define SPI_SCLK 18
#define SPI_MOSI 23
#define SPI_MISO 19

#define CS_DAC0 5
#define CS_DAC1 17
#define CS_DAC2 16
#define CS_DAC3 4
#define CS_ADC 21

#define DAC_VREF 2.5
#define D_16BIT 65535
#define M_SLOPE 4
#define C_OFFSET 2
#define NUM_BITS 16

const uint32_t DAC_FREQ = 8000000; //10MHz
const uint32_t ADC_FREQ = 1000000;  //8MHz

#endif


