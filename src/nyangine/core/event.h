#pragma once

typedef enum {
  NYA_EVENT_COUNT,
} NYA_EventType;

typedef struct {
  NYA_EventType type;
  bool          was_handled;
} NYA_Event;
