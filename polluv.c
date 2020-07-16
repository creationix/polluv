#include "polluv.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  uv_event_push(uv_event_get(uv_handle_get_loop((uv_handle_t*)timer)),
                (uv_event_t){UV_EVENT_TIMER_CB, {.timer = timer}});
}

void uv_event_close_cb(uv_handle_t* handle) {
  uv_event_push(uv_event_get(uv_handle_get_loop(handle)), (uv_event_t){UV_EVENT_CLOSE_CB, {.handle = handle}});
}
