#pragma once

/*
 * Enum class for managing finite state machine's states and events
 */
enum class State  { IDLE, SWEEP, STEADY, ERROR };

enum class Event {
  NO_EVENT, STOP, START_SWEEP, START_STEADY, 
  DONE_SWEEP, DONE_STEADY, ERROR, CONTINUE
};
