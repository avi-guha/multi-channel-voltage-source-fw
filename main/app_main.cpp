#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "coordinator.hpp"
#include "task_comms.hpp"
#include "log.hpp"


extern "C" void app_main(void) {

  data_queue = xQueueCreate(4, sizeof(DataLog));
  user_cmd_queue = xQueueCreate(4, sizeof(UserCmd));

  xTaskCreatePinnedToCore(Log::log_task, "Log", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(user_cmd_task, "Input", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(Coordinator::coordinator_task, "Coordinator", 3000, NULL, 2, NULL, 1);
}
