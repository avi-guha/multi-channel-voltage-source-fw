idf_component_register(
  SRCS "channel.cpp" "coordinator.cpp" "task_comms.cpp"
  INCLUDE_DIRS "include"
  REQUIRES adc
           dac
           driver_hal
)
