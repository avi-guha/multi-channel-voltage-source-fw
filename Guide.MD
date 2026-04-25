# ESP32 + AD5761 + AD7172-2 Bring-Up Guide

This repository now contains a small firmware scaffold that runs on an ESP32 and drives:

* `AD5761RARUZ` on a shared SPI bus as the output DAC
* `AD7172-2BRUZ` on the same shared SPI bus as the input ADC

The code is split so the device logic is shared and there are two entry paths:

* ESP-IDF: [main/app_main.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/main/app_main.cpp)
* Arduino: [arduino/esp32_ad5761_ad7172_demo/esp32_ad5761_ad7172_demo.ino](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/arduino/esp32_ad5761_ad7172_demo/esp32_ad5761_ad7172_demo.ino)

The core drivers live in:

* [include/ad5761.hpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/include/ad5761.hpp)
* [include/ad7172.hpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/include/ad7172.hpp)
* [src/ad5761.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/ad5761.cpp)
* [src/ad7172.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/ad7172.cpp)
* [src/shared_spi_bus.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/shared_spi_bus.cpp)

## 1. Hardware Assumptions

The firmware assumes the exact pinout from `ReadMe.md`:

* `SCLK` -> GPIO 18
* `MOSI` -> GPIO 23
* `MISO` -> GPIO 19
* `DAC CS / SYNC` -> GPIO 5
* `ADC CS` -> GPIO 4

Important hardware assumptions:

* `AD5761` is the non-`R` part. It needs a valid external reference on `VREFIN`.
* `AD5761` must have analog rails with enough headroom for `-5 V to +5 V` output.
* `AD7172-2` is configured for bipolar mode with the internal `2.5 V` reference enabled.
* `AD7172-2` channels are configured as `AIN0..AIN3` measured against `AIN4`.
* `AIN4` must be tied to your analog ground if you want single-ended readings.

## 2. SPI Settings Used

The shared bus switches parameters per peripheral:

* DAC: `10 MHz`, `SPI mode 2`
* ADC: `8 MHz`, `SPI mode 3`

Arduino path:

* The switching happens through `SPI.beginTransaction(SPISettings(...))` in [src/shared_spi_bus.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/shared_spi_bus.cpp)

ESP-IDF path:

* The switching happens through separate `spi_device_handle_t` configurations on the same bus
* Chip-select is driven manually so the ADC `DOUT/RDY` line can be polled while `CS` stays low

## 3. DAC Initialization Details

The DAC init sequence in [src/ad5761.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/ad5761.cpp) is:

1. Software full reset: `0x0F0000`
2. Control register write: `0x04020A`

`0x04020A` means:

* `CV = 01` -> clear code is midscale
* `PV = 01` -> power-up voltage is midscale
* `RA = 010` -> output range is `-5 V to +5 V`
* `B2C = 0` -> straight binary input coding for bipolar mode

Voltage writes use command `0x03xxxx`, which is write-and-update DAC.

Application behavior used by `setDACVoltage(float voltage)`:

* The firmware clamps requested values to `-2.5 V .. +2.5 V`
* The AD5761 itself stays configured for `-5 V .. +5 V` because this part does not offer a native `-2.5 V .. +2.5 V` range
* `-2.5 V` maps near code `0x4000`
* `0.0 V` maps near code `0x8000`
* `+2.5 V` maps near code `0xC000`

## 4. ADC Initialization Details

The ADC init sequence in [src/ad7172.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/ad7172.cpp) is:

1. Interface reset with 64 ones: eight bytes of `0xFF`
2. Read `ID` register at `0x07`
3. Write `IFMODE = 0x0040`
4. Write `ADCMODE = 0x8000`
5. Write `SETUPCON0 = 0x1020`
6. Write `FILTCON0 = 0x050E`
7. Enable channels:
   `CH0 = 0x8004`
   `CH1 = 0x8024`
   `CH2 = 0x8044`
   `CH3 = 0x8064`

What those settings mean:

* `IFMODE = 0x0040`
  `DATA_STAT = 1`, so every data read also returns the status byte
* `ADCMODE = 0x8000`
  internal `2.5 V` reference enabled, continuous conversion, internal oscillator
* `SETUPCON0 = 0x1020`
  bipolar output coding, internal reference selected
* `FILTCON0 = 0x050E`
  sinc5 + sinc1 filter with `ODR = 0x0E`, roughly `100.2 SPS`

## 5. How Channel Reads Work

`readADCChannel(uint8_t channel)` does this:

1. Pulls ADC `CS` low
2. Polls the shared `MISO` pin until `DOUT/RDY` goes low
3. Reads the ADC data register plus appended status
4. Checks the status byte to identify which channel produced that conversion
5. Repeats until the requested channel is seen
6. Converts the 24-bit offset-binary code to volts

Voltage conversion assumes the internal `2.5 V` reference:

* `0x800000` -> about `0 V`
* `0x000000` -> about `-2.5 V`
* `0xFFFFFF` -> about `+2.5 V`

If your design uses an external ADC reference instead, update:

* `app_config::kAdcReferenceVoltage` in [include/pin_config.hpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/include/pin_config.hpp)
* `kSetupCon0BipolarInternalRef` in [include/ad7172.hpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/include/ad7172.hpp)

## 6. Expected Runtime Behavior

The demo loop in [src/demo_app.cpp](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/src/demo_app.cpp):

* steps the DAC through `-2.5`, `-1.25`, `0.0`, `1.25`, `2.5` volts
* waits `50 ms`
* reads back all four ADC channels
* prints the measured voltages once per second

## 7. Debug Checklist

If the DAC output is wrong:

* Verify the DAC external reference is present and stable
* Verify DAC analog rails can actually support the configured hardware range of `-5 V to +5 V`
* Probe SPI and confirm the first two 24-bit writes are:
  `0x0F0000`
  `0x04020A`
* If you command `-2.5 V`, expect a DAC code near `0x4000`, not `0x0000`
* Check that `SYNC` returns high after each 24-bit transfer

If the ADC always returns `0xFFFFFF` or `0x000000`:

* Check the ADC reference wiring first
* Check that `AINx` voltages stay inside the allowed analog supply range
* Check the appended status byte for bit `6` (`ADC_ERROR`)
* Confirm `CS` stays low while polling `DOUT/RDY`

If the ADC never appears ready:

* Remember `DOUT/RDY` is tri-stated when `CS` is high
* Put a scope or logic analyzer on `ADC CS` and `MISO`
* Verify `ADC CS` is low during the ready poll
* Verify the reset burst is eight bytes of `0xFF`

If bus contention appears:

* Confirm only one chip select is ever low at a time
* Confirm the DAC and ADC are using different SPI modes as configured
* Confirm the `MISO` line is not being driven by the DAC when `DAC CS` is high

## 8. Build Notes

ESP-IDF project files:

* [CMakeLists.txt](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/CMakeLists.txt)
* [main/CMakeLists.txt](/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/main/CMakeLists.txt)

Typical ESP-IDF flow once `idf.py` is installed and exported:

```bash
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

Typical Arduino flow:

* open `arduino/esp32_ad5761_ad7172_demo/esp32_ad5761_ad7172_demo.ino`
* keep the companion wrapper files in the same sketch folder so the Arduino builder compiles the shared repository sources
* select an ESP32 board
* build and flash
