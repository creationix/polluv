#include "uv.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// POLLUV ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum uv_event_enum {
  UV_EVENT_NONE = 0,
  UV_EVENT_CLOSE_CB,
  UV_EVENT_TIMER_CB,
} uv_event_enum_t;

typedef union uv_event_args {
  uv_handle_t* handle;
  uv_timer_t* timer;
} uv_event_args_t;

typedef struct uv_event {
  uv_event_enum_t type;
  uv_event_args_t args;
} uv_event_t;

typedef struct uv_event_node {
  uv_event_t event;
  struct uv_event_node* next;
} uv_event_node_t;

typedef struct uv_event_loop {
  uv_loop_t* loop;
  uv_event_node_t* first;
  uv_event_node_t* last;
} uv_event_loop_t;

uv_event_loop_t* uv_event_get(uv_loop_t* ptr);
const char* uv_event_type_name(uv_event_enum_t type);
void uv_event_init(uv_event_loop_t* l, uv_loop_t* loop);
void uv_event_close(uv_event_loop_t* l);
void uv_event_push(uv_event_loop_t* l, uv_event_t evt);
uv_event_t uv_event_shift(uv_event_loop_t* l);
void uv_event_timer_cb(uv_timer_t* timer);
void uv_event_close_cb(uv_handle_t* handle);
