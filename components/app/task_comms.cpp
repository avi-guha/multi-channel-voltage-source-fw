#include <stdio.h>
#include <cstring>
#include "task_comms.hpp"

QueueHandle_t event_queue;
QueueHandle_t data_queue;
QueueHandle_t user_cmd_queue;

UserCmd cmd;
DataLog data;


void user_cmd_task(void* arg){
  printf("yo");

  char buffer[128];
  char* token;
  char* saveptr = nullptr;
  const char delimiter[] = ",";

  while(true){

    fgets(buffer, sizeof(buffer), stdin);

    cmd.channel_id = atoi(strtok_r(buffer, delimiter, &saveptr));

    token = strtok_r(buffer, delimiter, &saveptr);
    if (strcmp(token,"OFF")){
      cmd.mode = Mode::OFF;
    }
    else if (strcmp(token,"STEADY")){

      cmd.mode = Mode::STEADY;  
      cmd.param.Steady.voltage = atof(strtok_r(buffer, delimiter, &saveptr));
      cmd.param.Steady.duration = atof(strtok_r(buffer, delimiter, &saveptr));

      token = strtok_r(buffer, delimiter, &saveptr);

      if(strcmp(token, "Min")){
        cmd.param.Steady.time_unit = TimeUnit::MIN;
      }
      else if(strcmp(token, "Hour")){
        cmd.param.Steady.time_unit = TimeUnit::HOUR;
      }
      else if(strcmp(token, "Day")){
        cmd.param.Steady.time_unit = TimeUnit::DAY;
      }
      else if(strcmp(token, "Month")){
        cmd.param.Steady.time_unit = TimeUnit::MONTH;
      }
    }
    else if (strcmp(token,"SWEEP")){

      cmd.mode = Mode::SWEEP;  
      cmd.param.Sweep.range_in_V = atof(strtok_r(buffer, delimiter, &saveptr));
      cmd.param.Sweep.step_size = atof(strtok_r(buffer, delimiter, &saveptr));
    }

    xQueueSend(user_cmd_queue, &cmd, 0);
  }
}


void log_task (void* arg){
  while(true){
    if(xQueueReceive(data_queue, &data, 0) == pdTRUE){
      printf("data, %d, %d, %d, %.3f, %lu", 
          data.channel_id, 
          (int)data.mode, 
          data.voltage, 
          data.current, 
          (unsigned long)data.time
      );  }
  }
}
