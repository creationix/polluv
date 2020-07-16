// Use opaque pointers to all of libuv so we don't need to include it's headers.

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_timer_s uv_timer_t;
