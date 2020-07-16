#include <stdio.h>
#include <stdlib.h>

#include "polluv.h"

int main() {
  printf("Current libuv version = %s\n", uv_version_string());

  uv_event_loop_t l;
  uv_event_init(&l, uv_default_loop());

  uv_timer_t* timer = malloc(uv_handle_size(UV_TIMER));
  uv_timer_init(l.loop, timer);
  uv_timer_start(timer, uv_event_timer_cb, 100, 500);

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

  free(timer);
  uv_event_close(&l);

  return 0;
}
