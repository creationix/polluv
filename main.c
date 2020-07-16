#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uv.h"

#define container_of(ptr, type, member)               \
  ({                                                  \
    const typeof(((type*)0)->member)* __mptr = (ptr); \
    (type*)((char*)__mptr - offsetof(type, member));  \
  })

typedef struct uv_event_s uv_event_t;

typedef enum {
  UV_EVENT_NONE = 0,
  UV_EVENT_TIMER_CB,
} uv_event_enum_t;

typedef union {
  struct {
    uv_timer_t* timer;
  } timer_cb;
} uv_event_args_t;

struct uv_event_s {
  uv_event_enum_t type;
  uv_event_args_t args;
  uv_event_t* next;
};

typedef struct uv_event_loop_s {
  uv_loop_t loop;
  uv_event_t* first;
  uv_event_t* last;
} uv_event_loop_t;

const char* uv_event_type_name(uv_event_enum_t type) {
  switch (type) {
    case UV_EVENT_NONE:
      return "UV_EVENT_NONE";
    case UV_EVENT_TIMER_CB:
      return "UV_EVENT_TIMER_CB";
  }
  return NULL;
}

void uv_event_push(uv_event_loop_t* l, uv_event_enum_t type, uv_event_args_t args) {
  uv_event_t* evt = malloc(sizeof(uv_event_t));
  evt->type = type;
  evt->args = args;
  evt->next = NULL;
  if (l->last) {
    l->last->next = evt;
  } else {
    l->first = l->last = evt;
  }
}

uv_event_t uv_event_shift(uv_event_loop_t* l) {
  uv_run(&l->loop, UV_RUN_ONCE);

  uv_event_t evt;
  memset(&evt, 0, sizeof(evt));

  uv_event_t* first = l->first;
  if (first) {
    evt.type = first->type;
    evt.args = first->args;
    if (first->next) {
      l->first = first->next;
    } else {
      l->first = l->last = NULL;
    }
    free(first);
  }

  return evt;
}

#define get_container(ptr) container_of(ptr, uv_event_loop_t, loop)

static void uv_event_timer_cb(uv_timer_t* timer) {
  uv_event_push(get_container(timer->loop), UV_EVENT_TIMER_CB, (uv_event_args_t){.timer_cb = {timer}});
}

int main() {
  printf("Current libuv version = %s\n", uv_version_string());

  uv_event_loop_t l;
  uv_loop_init(&l.loop);
  l.first = l.last = NULL;

  uv_timer_t timer;
  uv_timer_init(&l.loop, &timer);
  uv_timer_start(&timer, uv_event_timer_cb, 1000, 0);

  while (true) {
    uv_event_t evt = uv_event_shift(&l);
    printf("Got event! %s\n", uv_event_type_name(evt.type));
    if (evt.type == UV_EVENT_NONE)
      break;
  }

  return 0;
}
