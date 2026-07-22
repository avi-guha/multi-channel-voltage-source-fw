/**
 * @file task_comms.cpp
 * @brief Implementation of the serial-input parser and CSV-out logger tasks.
 */

#include <stdio.h>
#include <cstring>
#include <esp_log.h>
#include "task_comms.hpp"


QueueHandle_t event_queue;
QueueHandle_t data_queue;
QueueHandle_t user_cmd_queue;

UserCmd cmd;


void user_cmd_task(void* arg){

  char buffer[128];
  const char delimiter[] = ",";

  while(true){

    if (fgets(buffer, sizeof(buffer), stdin) == nullptr){
      vTaskDelay(pdMS_TO_TICKS(20));
      continue;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
    char* ch_token;
    char* saveptr = nullptr;

    ch_token = strtok_r(buffer, delimiter, &saveptr);
    if (!ch_token) continue;
    cmd.channel_id = atoi(ch_token);

    char* mode_tok = strtok_r(nullptr, delimiter, &saveptr);
    if (!mode_tok) continue;

    if (strcmp(mode_tok,"OFF") == 0){
      cmd.mode = Mode::OFF;
    }
    else if (strcmp(mode_tok,"STEADY") == 0){

      char* voltage_tok = strtok_r(nullptr, delimiter, &saveptr);
      char* duration_tok = strtok_r(nullptr, delimiter, &saveptr);
      if (!voltage_tok || !duration_tok) continue;

      cmd.mode = Mode::STEADY;  
      cmd.param.Steady.voltage = atof(voltage_tok);
      cmd.param.Steady.duration = atof(duration_tok);

      char* time_tok = strtok_r(nullptr, delimiter, &saveptr);
      if (!time_tok) continue;
      
      if(strcmp(time_tok, "Min") == 0){
        cmd.param.Steady.time_unit = TimeUnit::MIN;
      }
      else if(strcmp(time_tok, "Hour") == 0){
        cmd.param.Steady.time_unit = TimeUnit::HOUR;
      }
      else if(strcmp(time_tok, "Day") == 0){
        cmd.param.Steady.time_unit = TimeUnit::DAY;
      }
      else if(strcmp(time_tok, "Month") == 0){
        cmd.param.Steady.time_unit = TimeUnit::MONTH;
      }
    }
    else if (strcmp(mode_tok,"SWEEP") == 0){

      cmd.mode = Mode::SWEEP;  
      cmd.param.Sweep.range_in_V = atof(strtok_r(nullptr, delimiter, &saveptr));
      cmd.param.Sweep.step_size = atof(strtok_r(nullptr, delimiter, &saveptr));
    }

    xQueueSend(user_cmd_queue, &cmd, 0);
  }
}


void log_task (void* arg){

  DataLog received_data;
  while(true){
    if(xQueueReceive(data_queue, &received_data, 1) == pdTRUE){
      printf("data, %d, %d, %.3f, %.5f, %lu \n", 
          received_data.channel_id, 
          (int)received_data.mode, 
          received_data.voltage, 
          received_data.current, 
          (unsigned long)received_data.time
      );  
    }
  }
}
