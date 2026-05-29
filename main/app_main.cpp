#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.hpp"
#include "demo_app.hpp"
#include "platform_abstraction.hpp"

extern "C" void app_main(void) {
  
  static DemoApp app;
  static FSM fsm;

  TaskHandle_t fsm_task_handle = NULL;
  TaskHandle_t adc_task_handle = NULL;
  TaskHandle_t dac_task_handle = NULL;
  TaskHandle_t sweep_task_handle = NULL;

  void log_task (void* arg);
  void usr_input_task (void* arg);
  void fsm_task (void* arg);
  void adc_task (void* arg);
  void dac_task (void* arg);
  void sweep_task (void* arg);
  

  xTaskCreatePinnedToCore(log_task, "log", 3000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(usr_input_task, "input", 3000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(fsm_task, "fsm", 3000, NULL, 2, &fsm_task_handle, 1);
  xTaskCreatePinnedToCore(adc_task, "adc", 3000, NULL, 5, &adc_task_handle, 1);
  xTaskCreatePinnedToCore(dac_task, "dac", 3000, NULL, 4, &dac_task_handle, 1);
  xTaskCreatePinnedToCore(sweep_task, "sweep", 3000, NULL, 5, &sweep_task_handle, 1);

  if (!fsm.begin()) {
    platform::logError("app_main", "application init failed");
    while (true) {
      platform::delayMs(1000);
    }
  }

  while (true) {
    fsm.begin();
  }
}
