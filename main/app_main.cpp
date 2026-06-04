#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "fsm.hpp"
#include "fsm_types.hpp"
#include "task_comms.hpp"
#include "log.hpp"


extern "C" void app_main(void) {

  event_queue = xQueueCreate(1, sizeof(Event));
  data_queue = xQueueCreate(1, sizeof(DataLog));
  user_cmd_queue = xQueueCreate(2, sizeof(UserCmd));

  TaskHandle_t fsm_task_handle = NULL;

  xTaskCreatePinnedToCore(Log::log_task, "Log", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(user_cmd_task, "Input", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(FSM::fsm_task, "FSM", 3000, NULL, 3, &fsm_task_handle, 1);
}
