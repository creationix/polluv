
# These work!
CC = clang
# CC = gcc
# CC = musl-gcc -static


# These do not work...
# CC = ${HOME}/zig-linux-x86_64-0.6.0+6b103324d/zig cc -target native-native-musl
# CC = ${HOME}/zig-linux-x86_64-0.6.0+6b103324d/zig cc -target x86_64-linux-gnu.2.12
# CC = ${HOME}/zig-linux-x86_64-0.6.0+6b103324d/zig cc

# They all error with:
#
#     Compile C Objects [36/36] random-sysctl-linux.c...lld: error: undefined symbol: fcntl64
#     >>> referenced by core.c
#     >>>               zig-cache/o/nlb2OM8TPsUQIc_XQNmBz4fpS42PjAahZoSzC1KjNaTwF59ur_l8CNhDPZ9IcATL/core.o:(uv__nonblock_fcntl)
#     >>> referenced by core.c
#     >>>               zig-cache/o/nlb2OM8TPsUQIc_XQNmBz4fpS42PjAahZoSzC1KjNaTwF59ur_l8CNhDPZ9IcATL/core.o:(uv__nonblock_fcntl)
#     >>> referenced by core.c
#     >>>               zig-cache/o/nlb2OM8TPsUQIc_XQNmBz4fpS42PjAahZoSzC1KjNaTwF59ur_l8CNhDPZ9IcATL/core.o:(uv__cloexec_fcntl)
#     >>> referenced by core.c
#     >>>               zig-cache/o/nlb2OM8TPsUQIc_XQNmBz4fpS42PjAahZoSzC1KjNaTwF59ur_l8CNhDPZ9IcATL/core.o:(uv__cloexec_fcntl)
#     >>> referenced by pipe.c
#     >>>               zig-cache/o/jhXTjrrhm43hVXUukCHp92dWEIPwB8xZIJSLCh9zpAC-De0Q4JDPpIOMbVI38yqD/pipe.o:(uv_pipe_open)
#     >>> referenced by process.c
#     >>>               zig-cache/o/o436TuIX3xCh4SdlyVZMcnyNCMEYjh9ncu33riA1F0KCGXM-f7L_seyx3wnMPVci/process.o:(uv__process_child_init)
#     >>> did you mean: fcntl64
#     >>> defined in: /home/tim/.cache/zig/stage1/h/pVSItZEIxIXx_ZQQwt9SLNNbPH8xxZp1plbirUK6SOmyZ4FTf2EOP08onozfGP03/libc.so.6.0.0

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


main: main.c $(addprefix ${LIBUV_PATH}/, ${LIBUV_SRC_FILES})
	$(CC) -o $@ $^ -Ilibuv/include ${LIBUV_CFLAGS} $(addprefix -I, ${LIBUV_C_INCLUDES})
