#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "coordinator.hpp"
#include "task_comms.hpp"


extern "C" void app_main(void) {

  data_queue = xQueueCreate(4, sizeof(DataLog));
  user_cmd_queue = xQueueCreate(4, sizeof(UserCmd));

  xTaskCreatePinnedToCore(log_task, "Log", 7000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(user_cmd_task, "Input", 7000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(Coordinator::coordinator_task, "Coordinator", 70000, NULL, 2, NULL, 1);
}
