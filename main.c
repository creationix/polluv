#include "main.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uv.h"

uv_event_loop_t* uv_event_get(uv_loop_t* ptr) {
  uv_event_loop_t* l = (uv_event_loop_t*)uv_loop_get_data(ptr);
  return l->loop == ptr ? l : NULL;
}

const char* uv_event_type_name(uv_event_enum_t type) {
  switch (type) {
    case UV_EVENT_NONE:
      return "UV_EVENT_NONE";
    case UV_EVENT_CLOSE_CB:
      return "UV_EVENT_CLOSE_CB";
    case UV_EVENT_TIMER_CB:
      return "UV_EVENT_TIMER_CB";
  }
  return NULL;
}

void uv_event_init(uv_event_loop_t* l, uv_loop_t* loop) {
  uv_loop_set_data(loop, l);
  l->loop = loop;
  l->first = NULL;
  l->last = NULL;
}

void uv_event_close(uv_event_loop_t* l) {
  uv_loop_set_data(l->loop, NULL);
}

void uv_event_push(uv_event_loop_t* l, uv_event_t evt) {
  uv_event_node_t* node = malloc(sizeof(uv_event_node_t));
  node->event = evt;
  node->next = NULL;
  printf("push event %p %s\n", node, uv_event_type_name(node->event.type));
  if (l->last) {
    l->last->next = node;
  } else {
    l->first = l->last = node;
  }
}

uv_event_t uv_event_shift(uv_event_loop_t* l) {
  if (!l->first)
    uv_run(l->loop, UV_RUN_ONCE);

  uv_event_t evt;
  memset(&evt, 0, sizeof(evt));

  uv_event_node_t* node = l->first;
  printf("node %p type=%s\n", node, node ? uv_event_type_name(node->event.type) : "");
  if (!node)
    return evt;

  evt = node->event;
  if (l->last == l->first) {
    l->first = l->last = node->next;
  } else {
    l->first = node->next;
  }
  free(node);

  return evt;
}

void uv_event_timer_cb(uv_timer_t* timer) {
  uv_event_push(uv_event_get(timer->loop), (uv_event_t){UV_EVENT_TIMER_CB, {.timer = timer}});
}

void uv_event_close_cb(uv_handle_t* handle) {
  uv_event_push(uv_event_get(handle->loop), (uv_event_t){UV_EVENT_CLOSE_CB, {.handle = handle}});
}

int main() {
  printf("Current libuv version = %s\n", uv_version_string());

  uv_event_loop_t l;
  uv_event_init(&l, uv_default_loop());

  uv_timer_t timer;
  uv_timer_init(l.loop, &timer);
  uv_timer_start(&timer, uv_event_timer_cb, 100, 500);

  uv_event_t evt;
  do {
    evt = uv_event_shift(&l);
    printf("Got event! %s\n", uv_event_type_name(evt.type));
    switch (evt.type) {
      case UV_EVENT_TIMER_CB: {
        uint64_t delay = uv_timer_get_repeat(evt.args.timer) - 100;
        printf("UV_EVENT_TIMER_CB delay=%lu\n", delay);
        if (delay > 0) {
          uv_timer_set_repeat(evt.args.timer, delay);
        } else {
          uv_close((uv_handle_t*)evt.args.timer, uv_event_close_cb);
        }
        break;
      }
      default:
        printf("TODO: handle type %d\n", evt.type);
        break;
    }

  } while (evt.type != UV_EVENT_NONE);

  uv_event_close(&l);

  return 0;
}
