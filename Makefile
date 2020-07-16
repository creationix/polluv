# CC = zig cc
CC = ${HOME}/zig-linux-x86_64-0.6.0+6b103324d/zig cc -target native-native-musl

LIBUV_PATH := libuv

LIBUV_CFLAGS := \
	-pthread \
	-D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-D_GNU_SOURCE \
	-D_POSIX_C_SOURCE=200112 \
	-ldl -lrt \


LIBUV_C_INCLUDES := \
	$(LIBUV_PATH)/include \
	$(LIBUV_PATH)/src \

LIBUV_SRC_FILES := \
	src/fs-poll.c \
	src/idna.c \
	src/inet.c \
	src/random.c \
	src/strscpy.c \
	src/threadpool.c \
	src/timer.c \
	src/uv-common.c \
	src/uv-data-getter-setters.c \
	src/version.c \
	src/unix/async.c \
	src/unix/core.c \
	src/unix/dl.c \
	src/unix/fs.c \
	src/unix/getaddrinfo.c \
	src/unix/getnameinfo.c \
	src/unix/loop-watcher.c \
	src/unix/loop.c \
	src/unix/pipe.c \
	src/unix/poll.c \
	src/unix/process.c \
	src/unix/random-devurandom.c \
	src/unix/signal.c \
	src/unix/stream.c \
	src/unix/tcp.c \
	src/unix/thread.c \
	src/unix/tty.c \
	src/unix/udp.c \
	src/unix/proctitle.c \
	src/unix/linux-core.c \
	src/unix/linux-inotify.c \
	src/unix/linux-syscalls.c \
	src/unix/procfs-exepath.c \
	src/unix/random-getrandom.c \
	src/unix/random-sysctl-linux.c \


main: main.c $($(addprefix ${LIBUV_PATH}/, ${LIBUV_C_INCLUDES}))
	$(CC) -o $@ $^ -Ilibuv/include
