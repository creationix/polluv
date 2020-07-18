//
#include "uv.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// POLLUV ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum polluv_type {
  POLLUV_NONE = 0,
  POLLUV_CLOSE_CB,
  POLLUV_TIMER_CB,
} polluv_type_t;

typedef union polluv_args {
  uv_handle_t* handle;
  uv_timer_t* timer;
} polluv_args_t;

typedef struct polluv_event {
  polluv_type_t type;
  polluv_args_t args;
} polluv_event_t;

typedef struct polluv_node {
  polluv_event_t event;
  struct polluv_node* next;
} polluv_node_t;

typedef struct polluv_state {
  uv_loop_t* loop;
  polluv_node_t* first;
  polluv_node_t* last;
} polluv_state_t;

polluv_state_t* polluv_get(uv_loop_t* ptr);
const char* polluv_type_name(polluv_type_t type);
void polluv_init(polluv_state_t* l, uv_loop_t* loop);
void polluv_close(polluv_state_t* l);
void polluv_push(polluv_state_t* l, polluv_event_t evt);
polluv_event_t polluv_shift(polluv_state_t* l);
void polluv_timer_cb(uv_timer_t* timer);
void polluv_close_cb(uv_handle_t* handle);
