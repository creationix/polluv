#include "polluv.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

polluv_state_t* polluv_get(uv_loop_t* ptr) {
  polluv_state_t* l = (polluv_state_t*)uv_loop_get_data(ptr);
  return l->loop == ptr ? l : NULL;
}

const char* polluv_type_name(polluv_type_t type) {
  switch (type) {
    case POLLUV_NONE:
      return "NONE";
    case POLLUV_CLOSE_CB:
      return "CLOSE_CB";
    case POLLUV_TIMER_CB:
      return "TIMER_CB";
  }
  return NULL;
}

void polluv_init(polluv_state_t* l, uv_loop_t* loop) {
  uv_loop_set_data(loop, l);
  l->loop = loop;
  l->first = NULL;
  l->last = NULL;
}

void polluv_close(polluv_state_t* l) {
  uv_loop_set_data(l->loop, NULL);
}

void polluv_push(polluv_state_t* l, polluv_event_t evt) {
  polluv_node_t* node = malloc(sizeof(polluv_node_t));
  node->event = evt;
  node->next = NULL;
  if (l->last) {
    l->last->next = node;
  } else {
    l->first = l->last = node;
  }
}

polluv_event_t polluv_shift(polluv_state_t* l) {
  if (!l->first)
    uv_run(l->loop, UV_RUN_ONCE);

  polluv_event_t evt;
  memset(&evt, 0, sizeof(evt));

  polluv_node_t* node = l->first;
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

void polluv_timer_cb(uv_timer_t* timer) {
  polluv_push(polluv_get(uv_handle_get_loop((uv_handle_t*)timer)), (polluv_event_t){POLLUV_TIMER_CB, {.timer = timer}});
}

void polluv_close_cb(uv_handle_t* handle) {
  polluv_push(polluv_get(uv_handle_get_loop(handle)), (polluv_event_t){POLLUV_CLOSE_CB, {.handle = handle}});
}
