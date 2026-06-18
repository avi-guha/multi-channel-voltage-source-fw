idf_component_register(
  SRCS  "task_comms.cpp"
  INCLUDE_DIRS "include"
  REQUIRES adc
           dac
           driver_hal
)
