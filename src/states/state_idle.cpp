#include "states/state_idle.hpp"
#include "task_comms.hpp"
#include "fsm_types.hpp"


void idle_enter(){

}

/* ToDo 
 * Is xQueueCreateStatic a better choice?
 * Do I just have infinite loop until queue arrives? Is this non-blocking.
 * Where do I make and send user_cmd_queue? Idle state is receiving it
 * Do I make the struct in every class that needs to receive?
 *
 */
void idle_run(){
  while(user_cmd_queue == 0){
    continue;
  } 
  xQueueReceive(user_cmd_queue, &UserCmd, );
  xQueueSend(event_queue, &event, pdMS_TO_TICKS(100)); 
}

void idle_exit(){
  
}
