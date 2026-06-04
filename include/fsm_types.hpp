#pragma once


enum class State  { IDLE, SWEEP, STEADY, ERROR };

enum class Event {
  NO_EVENT, STOP, START_SWEEP, START_STEADY, 
  DONE_SWEEP, DONE_STEADY, ERROR, CONTINUE
};
