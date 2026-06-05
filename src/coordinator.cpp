#include "coordinator.hpp"
#include "channel.hpp"

static constexpr uint8_t NUM_CHANNELS = 4;

static Channel channel[NUM_CHANNELS];
static struct UserCmd cmd;


void coordinator_task(void* arg){

  while(true){ 

    while(xQueueReceive(user_cmd_queue, &cmd, 0) == pdTRUE){
      channel[cmd.channel_id].update(cmd);
    }

    for(int i = 0; i < NUM_CHANNELS; i++){

      switch(channel[i].mode){

        case Mode::OFF:
          break;

        case Mode::STEADY:
          channel[i].steady_run();
          break;

        case Mode::SWEEP:
          channel[i].sweep_run();
          break;
      }
    }
  }
}
