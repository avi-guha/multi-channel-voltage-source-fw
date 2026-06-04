#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "fsm.hpp"
#include "log.hpp"
#include "sweep.hpp"
#include "ad5761.hpp"
#include "ad7172.hpp"

extern "C" void app_main(void) {

  event_queue = xQueueCreate(1, sizeof(Event));
  user_cmd_queue = xQueueCreate(3, sizeof(UserCmd));

  TaskHandle_t fsm_task_handle = NULL;
  TaskHandle_t adc_task_handle = NULL;
  TaskHandle_t dac_task_handle = NULL;
  TaskHandle_t sweep_task_handle = NULL;

  xTaskCreatePinnedToCore(Log::log_task, "Log", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(user_cmd_task, "Input", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(FSM::fsm_task, "FSM", 3000, NULL, 3, &fsm_task_handle, 1);
  xTaskCreatePinnedToCore(Ad7172_2::ADC_task, "Adc", 3000, NULL, 5, &adc_task_handle, 1);
  xTaskCreatePinnedToCore(Ad5761::DAC_task, "Dac", 3000, NULL, 4, &dac_task_handle, 1);
  xTaskCreatePinnedToCore(sweep_task, "Sweep", 3000, NULL, 5, &sweep_task_handle, 1);
}
