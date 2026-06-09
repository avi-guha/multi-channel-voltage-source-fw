#include "log.hpp"
#include "task_comms.hpp"

DataLog data;

void log_task (void* arg){
  while(true){
    if(xQueueReceive(data_queue, &data, 0) == pdTRUE){

      printf("data, %d, %d, %d, %.3f, %d", 
          data.channel_id, 
          data.mode, 
          data.voltage, 
          data.current, 
          data.time);
    }
  }
}

