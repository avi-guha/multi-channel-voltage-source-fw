#include <stdio.h>
#include <cstring>
#include <esp_log.h>
#include "task_comms.hpp"

static constexpr const char* TAG = "comms";

QueueHandle_t event_queue;
QueueHandle_t data_queue;
QueueHandle_t user_cmd_queue;

UserCmd cmd;
DataLog data;


void user_cmd_task(void* arg){

  char buffer[128];
  char* token;
  char* saveptr = nullptr;
  const char delimiter[] = ",";

  while(true){

    if (fgets(buffer, sizeof(buffer), stdin) == nullptr){
      vTaskDelay(pdMS_TO_TICKS(20));
      continue;
    }
    cmd.channel_id = atoi(strtok_r(buffer, delimiter, &saveptr));
      ESP_LOGI(TAG, "ch: %d", cmd.channel_id);

    token = strtok_r(nullptr, delimiter, &saveptr);
    int ret = strcmp(token,"OFF");
    if (strcmp(token,"OFF") == 0){
      cmd.mode = Mode::OFF;
    }
    else if (strcmp(token,"STEADY") == 0){

      cmd.mode = Mode::STEADY;  
      cmd.param.Steady.voltage = atof(strtok_r(nullptr, delimiter, &saveptr));
      cmd.param.Steady.duration = atof(strtok_r(nullptr, delimiter, &saveptr));

      token = strtok_r(nullptr, delimiter, &saveptr);

      if(strcmp(token, "Min") == 0){
        cmd.param.Steady.time_unit = TimeUnit::MIN;
      }
      else if(strcmp(token, "Hour") == 0){
        cmd.param.Steady.time_unit = TimeUnit::HOUR;
      }
      else if(strcmp(token, "Day") == 0){
        cmd.param.Steady.time_unit = TimeUnit::DAY;
      }
      else if(strcmp(token, "Month") == 0){
        cmd.param.Steady.time_unit = TimeUnit::MONTH;
      }
    }
    else if (strcmp(token,"SWEEP") == 0){

      cmd.mode = Mode::SWEEP;  
      cmd.param.Sweep.range_in_V = atof(strtok_r(nullptr, delimiter, &saveptr));
      cmd.param.Sweep.step_size = atof(strtok_r(nullptr, delimiter, &saveptr));
    }

    xQueueSend(user_cmd_queue, &cmd, 0);
  }
}


void log_task (void* arg){
  while(true){
    if(xQueueReceive(data_queue, &data, portMAX_DELAY) == pdTRUE){
      // printf("data, %d, %d, %.3f, %.3f, %lu \n", 
      //     data.channel_id, 
      //     (int)data.mode, 
      //     data.voltage, 
      //     data.current, 
      //     (unsigned long)data.time
      // );  
    }
  }
}
