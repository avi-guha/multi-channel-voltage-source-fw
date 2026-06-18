#include "spi_configs.h"
#include "pin_config.h"
#include "no_os_esp32_spi.h"

#define AD7172_2_INIT
#include "ad7172_2_regs.h"

spi_config_extra_t config_extra = {
  .host = SPI2_HOST,
  .bus = {
    .sclk_io_num = SPI_SCLK,
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = SPI_MISO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 32
  }
};


struct no_os_spi_platform_ops platform_ops = {
  .init = esp32_spi_init,
  .write_and_read = esp32_spi_write_read,
  .transfer = esp32_spi_transfer,
  .transfer_dma = NULL,
  .transfer_dma_async = NULL,
  .remove = esp32_spi_remove,
  .transfer_abort = esp32_spi_transfer_abort
};


struct no_os_spi_init_param spi_config = {
  .device_id = 5,
  .max_speed_hz = DAC_FREQ,
  .chip_select = 0,
  .mode = NO_OS_SPI_MODE_3,
  .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
  .lanes = NO_OS_SPI_SINGLE_LANE,
  .platform_ops = &platform_ops,
  .platform_delays = {
    .cs_delay_first = 0,
    .cs_delay_last = 0
  },
  .extra = &config_extra,
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


