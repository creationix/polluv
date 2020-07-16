//
#include <stdint.h>
#include <unistd.h>

// Use opaque pointers to all of libuv so we don't need to include it's headers.
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_buf_s uv_buf_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_poll_s uv_poll_t;
typedef struct uv_timer_s uv_timer_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_prepare_s uv_prepare_t;
typedef struct uv_check_s uv_check_t;
typedef struct uv_idle_s uv_idle_t;
typedef struct uv_process_s uv_process_t;
typedef struct uv_stream_s uv_stream_t;
typedef struct uv_getaddrinfo_s uv_getaddrinfo_t;
typedef struct uv_getnameinfo_s uv_getnameinfo_t;
typedef struct uv_req_s uv_req_t;
typedef struct uv_write_s uv_write_t;
typedef struct uv_connect_s uv_connect_t;
typedef struct uv_shutdown_s uv_shutdown_t;
typedef struct uv_fs_s uv_fs_t;
typedef struct uv_work_s uv_work_t;
typedef struct uv_random_s uv_random_t;

typedef int uv_os_fd_t;

typedef enum {
  UV_UNKNOWN_HANDLE = 0,
  UV_ASYNC,
  UV_CHECK,
  UV_FS_EVENT,
  UV_FS_POLL,
  UV_HANDLE,
  UV_IDLE,
  UV_NAMED_PIPE,
  UV_POLL,
  UV_PREPARE,
  UV_PROCESS,
  UV_STREAM,
  UV_TCP,
  UV_TIMER,
  UV_TTY,
  UV_UDP,
  UV_SIGNAL,
  UV_FILE,
  UV_HANDLE_TYPE_MAX
} uv_handle_type;

typedef enum {
  UV_UNKNOWN_REQ = 0,
  UV_REQ,
  UV_CONNECT,
  UV_WRITE,
  UV_SHUTDOWN,
  UV_UDP_SEND,
  UV_FS,
  UV_WORK,
  UV_GETADDRINFO,
  UV_GETNAMEINFO,
  UV_RANDOM,
  UV_REQ_TYPE_MAX
} uv_req_type;

typedef void (*uv_alloc_cb)(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
typedef void (*uv_read_cb)(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
typedef void (*uv_write_cb)(uv_write_t* req, int status);
typedef void (*uv_connect_cb)(uv_connect_t* req, int status);
typedef void (*uv_shutdown_cb)(uv_shutdown_t* req, int status);
typedef void (*uv_connection_cb)(uv_stream_t* server, int status);
typedef void (*uv_close_cb)(uv_handle_t* handle);
typedef void (*uv_poll_cb)(uv_poll_t* handle, int status, int events);
typedef void (*uv_timer_cb)(uv_timer_t* handle);
typedef void (*uv_async_cb)(uv_async_t* handle);
typedef void (*uv_prepare_cb)(uv_prepare_t* handle);
typedef void (*uv_check_cb)(uv_check_t* handle);
typedef void (*uv_idle_cb)(uv_idle_t* handle);
typedef void (*uv_exit_cb)(uv_process_t*, int64_t exit_status, int term_signal);
typedef void (*uv_walk_cb)(uv_handle_t* handle, void* arg);
typedef void (*uv_fs_cb)(uv_fs_t* req);
typedef void (*uv_work_cb)(uv_work_t* req);
typedef void (*uv_after_work_cb)(uv_work_t* req, int status);
// typedef void (*uv_getaddrinfo_cb)(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
typedef void (*uv_getnameinfo_cb)(uv_getnameinfo_t* req, int status, const char* hostname, const char* service);
typedef void (*uv_random_cb)(uv_random_t* req, int status, void* buf, size_t buflen);

typedef enum { UV_RUN_DEFAULT = 0, UV_RUN_ONCE, UV_RUN_NOWAIT } uv_run_mode;

// Also expose any libuv functions we want callable from ffi.
unsigned int uv_version(void);
const char* uv_version_string(void);
void uv_library_shutdown(void);
uv_loop_t* uv_default_loop(void);
int uv_loop_init(uv_loop_t* loop);
int uv_loop_close(uv_loop_t* loop);
size_t uv_loop_size(void);
int uv_loop_alive(const uv_loop_t* loop);
int uv_run(uv_loop_t*, uv_run_mode mode);
void uv_stop(uv_loop_t*);
void* uv_loop_get_data(const uv_loop_t*);
void uv_loop_set_data(uv_loop_t*, void* data);

size_t uv_handle_size(uv_handle_type type);
uv_handle_type uv_handle_get_type(const uv_handle_t* handle);
const char* uv_handle_type_name(uv_handle_type type);
void* uv_handle_get_data(const uv_handle_t* handle);
uv_loop_t* uv_handle_get_loop(const uv_handle_t* handle);
void uv_handle_set_data(uv_handle_t* handle, void* data);
size_t uv_req_size(uv_req_type type);
void* uv_req_get_data(const uv_req_t* req);
void uv_req_set_data(uv_req_t* req, void* data);
uv_req_type uv_req_get_type(const uv_req_t* req);
const char* uv_req_type_name(uv_req_type type);
int uv_is_active(const uv_handle_t* handle);
void uv_walk(uv_loop_t* loop, uv_walk_cb walk_cb, void* arg);
void uv_close(uv_handle_t* handle, uv_close_cb close_cb);
int uv_send_buffer_size(uv_handle_t* handle, int* value);
int uv_recv_buffer_size(uv_handle_t* handle, int* value);
int uv_fileno(const uv_handle_t* handle, uv_os_fd_t* fd);
uv_buf_t uv_buf_init(char* base, unsigned int len);

void uv_ref(uv_handle_t*);
void uv_unref(uv_handle_t*);
int uv_has_ref(const uv_handle_t*);
void uv_update_time(uv_loop_t*);
uint64_t uv_now(const uv_loop_t*);
int uv_backend_fd(const uv_loop_t*);
int uv_backend_timeout(const uv_loop_t*);

int uv_timer_init(uv_loop_t*, uv_timer_t* handle);
int uv_timer_start(uv_timer_t* handle, uv_timer_cb cb, uint64_t timeout, uint64_t repeat);
int uv_timer_stop(uv_timer_t* handle);
int uv_timer_again(uv_timer_t* handle);
void uv_timer_set_repeat(uv_timer_t* handle, uint64_t repeat);
uint64_t uv_timer_get_repeat(const uv_timer_t* handle);
