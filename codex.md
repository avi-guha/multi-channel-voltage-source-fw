# Firmware Generation Request: ESP32 with 4x AD5761 DAC and AD7172-2 ADC

Please write firmware in C++ for the ESP32 (compatible with the Arduino core / ESP-IDF) to control four AD5761RARUZ DACs and one AD7172-2BRUZ ADC over a shared SPI bus.

## Hardware Pinout
* **SPI SCLK:** GPIO 18
* **SPI MOSI:** GPIO 23
* **SPI MISO:** GPIO 19
* **DAC 1 / CS1 (SYNC):** GPIO 5
* **DAC 2 / CS2 (SYNC):** GPIO 17
* **DAC 3 / CS3 (SYNC):** GPIO 16
* **DAC 4 / CS4 (SYNC):** GPIO 4
* **ADC / CS5:** GPIO 21

## SPI Bus Configuration
The devices share an SPI bus but have different timing requirements. Please implement SPI transactions using `SPI.beginTransaction(SPISettings(...))` to dynamically switch parameters between peripheral calls.
* **AD5761 DAC SPI:** 24-bit shift register, Data clocked in on falling edge (SPI Mode 2 or as per AD5761 datasheet), max frequency 50 MHz (use 10 MHz for safety).
* **AD7172 ADC SPI:** Data clocked on falling edge, default high idle (SPI Mode 3), max frequency 20 MHz (use 8 MHz for safety).

## DAC Requirements (AD5761RARUZ)
1.  **Hardware State:** Each DAC `~LDAC` is hardwired to GND. The output updates automatically on the rising edge of its `CS` line. `~RESET` and `~CLEAR` are not physically connected.
2.  **Initialization:** * Perform a software reset by writing the Software Reset command (0x0F0000).
    * Write to the Control Register to configure the output range appropriately for bipolar operation (-5V to +5V) and enable the output.
3.  **Operation:** * Write a function `void setDACVoltage(float voltage)` for each DAC instance that maps a voltage (from -5V to +5V) to the 16-bit register and transmits the 24-bit command (Address + Data) over SPI using CS1-CS4.

## ADC Requirements (AD7172-2BRUZ)
1.  **Hardware State:** Bipolar supply (+2.5V to -2.5V). The `DOUT` pin doubles as the `~RDY` pin.
2.  **Initialization:**
    * Reset the ADC by writing 64 consecutive `1`s over the SPI bus.
    * Configure the ADC Setup Register for bipolar operation.
    * Configure the Channel Registers to enable inputs AIN0 through AIN3 (referenced to AIN4, which is grounded).
3.  **Operation:**
    * Write a function `float readADCChannel(uint8_t channel)` that polls the `~RDY` state (via the MISO line while CS is low), reads the 24-bit data register when ready, and converts the two's complement or offset binary 24-bit value to a real-world voltage.

## Code Structure
* Use modular, well-commented code.
* Abstract the SPI read/write functions into separate helper methods for the DAC and ADC to keep the code clean.
* Provide a basic `setup()` loop that initializes all four DACs plus the ADC and a `loop()` that sequentially steps DAC1-DAC4 through a few voltages and reads the ADC channels back to the Serial monitor.

## User Guide
* when done with the code, generate a Guide.MD that will be easy for the user of this software to follow and debug.
* refer to the specific parts and particular debugging steps.
* your code will be reviewed by claude when finished. 
