#include "spi_configs.h"
#include "pin_config.h"
#include "no_os_esp32_spi.h"

#define AD7172_2_INIT
#include "ad7172_2_regs.h"


esp32_spi_config_t spi_config = {
  .host = SPI2_HOST,
  .bus = {
    .sclk_io_num = SPI_SCLK,
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = SPI_MISO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 0
  }
};

struct no_os_spi_platform_ops platform_ops = {
  .init = esp32_spi_init,
  .write_and_read = esp32_spi_write_read,
  .transfer = NULL,
  .transfer_dma = NULL,
  .transfer_dma_async = NULL,
  .remove = esp32_spi_remove,
  .transfer_abort = NULL
};

struct no_os_spi_init_param dac0_config = {
  .device_id = 0,
  .max_speed_hz = DAC_FREQ,
  .chip_select = CS_DAC0,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .parent = NULL
};

struct no_os_spi_init_param dac1_config = {
  .device_id = 1,
  .max_speed_hz = DAC_FREQ,
  .chip_select = CS_DAC1,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .parent = NULL
};

struct no_os_spi_init_param dac2_config = {
  .device_id = 2,
  .max_speed_hz = DAC_FREQ,
  .chip_select = CS_DAC2,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .parent = NULL
};

struct no_os_spi_init_param dac3_config = {
  .device_id = 3,
  .max_speed_hz = DAC_FREQ,
  .chip_select = CS_DAC0,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .parent = NULL
};


//ADC config
struct no_os_spi_init_param adc_config = {
  .device_id = 5,
  .max_speed_hz = ADC_FREQ,
  .chip_select = CS_ADC,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .parent = NULL
};

const uint8_t ad7172_2_num_regs = sizeof(ad7172_2_regs) / sizeof(ad7172_2_regs[0]);


struct ad717x_channel_map chan_map[NUM_CHANNELS] = {
  [0] = {
    .channel_enable = true,
    .setup_sel = 0,
    .analog_inputs = {
      .ainp = {
        .pos_analog_input = AIN0,
        .neg_analog_input = AIN4
      }
    }
  }, 
  [1] = {
    .channel_enable = true,
    .setup_sel = 0,
    .analog_inputs = {
      .ainp = {
        .pos_analog_input = AIN1,
        .neg_analog_input = AIN4
      }
    }
  }, 
  [2] = {
    .channel_enable = true,
    .setup_sel = 0,
    .analog_inputs = {
      .ainp = {
        .pos_analog_input = AIN2,
        .neg_analog_input = AIN4
      }
    }
  }, 
  [3] = {
    .channel_enable = true,
    .setup_sel = 0,
    .analog_inputs = {
      .ainp = {
        .pos_analog_input = AIN3,
        .neg_analog_input = AIN4
      }
    } 
  }
};


struct ad717x_channel_setup setup[NUM_CHANNELS] = {
  [0] = {
    .bi_unipolar = true,
    .ref_buff = false,
    .input_buff = false,
    .ref_source = EXTERNAL_REF
  },
  [1] = {
    .bi_unipolar = true,
    .ref_buff = false,
    .input_buff = false,
    .ref_source = EXTERNAL_REF
  },
  [2] = {
    .bi_unipolar = true,
    .ref_buff = false,
    .input_buff = false,
    .ref_source = EXTERNAL_REF
  },
  [3] = {
    .bi_unipolar = true,
    .ref_buff = false,
    .input_buff = false,
    .ref_source = EXTERNAL_REF
  }
};


uint32_t pga[NUM_CHANNELS] = {[0] = 100, [1] = 100, [2] = 100, [3] = 100};


struct ad717x_filtcon filtcon[NUM_CHANNELS] = {
  [0] = {
    .sinc3_map = false,
    .enhfilten = false,
    .oder = sinc5_sinc1,
    .odr = sps_1007
  },
  [1] = {
    .sinc3_map = false,
    .enhfilten = false,
    .oder = sinc5_sinc1,
    .odr = sps_1007
  },
  [2] = {
    .sinc3_map = false,
    .enhfilten = false,
    .oder = sinc5_sinc1,
    .odr = sps_1007
  },
  [3] = {
    .sinc3_map = false,
    .enhfilten = false,
    .oder = sinc5_sinc1,
    .odr = sps_1007
  }
};


