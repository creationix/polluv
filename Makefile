
# These work!
# CC = clang
# CC = gcc -g
CC = musl-gcc -Os -static -Wl,--export-dynamic


# These do not work...
# CC = ${HOME}/zig-linux-x86_64-0.6.0+01ab167ce/zig cc -target native-native-musl
# CC = ${HOME}/zig-linux-x86_64-0.6.0+01ab167ce/zig cc -target x86_64-linux-gnu.2.12
# CC = ${HOME}/zig-linux-x86_64-0.6.0+01ab167ce/zig cc

LIBUV_PATH := libuv

LIBUV_CFLAGS := \
	-pthread \
	-D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-D_GNU_SOURCE \
	-D_POSIX_C_SOURCE=200112 \
	-Wall -Werror \

LIBUV_LDFLAGS := \
	-pthread \
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


LIBUV_OBJ_FILES=$(join $(addprefix obj/libuv/, $(dir $(LIBUV_SRC_FILES))), $(notdir $(LIBUV_SRC_FILES:.c=.o)))

all: test-ffi test-leak

obj/polluv.o: polluv.c polluv.h
	mkdir -p $(dir $@)
	$(CC) -fPIC -c $< -o $@ ${LIBUV_CFLAGS}

obj/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -fPIC -c $< -o $@ $(addprefix -I, ${LIBUV_C_INCLUDES}) ${LIBUV_CFLAGS}

libpolluv.so: obj/polluv.o ${LIBUV_OBJ_FILES}
	$(CC) -fPIC -shared $^ -o $@

test: test.c obj/polluv.o ${LIBUV_OBJ_FILES}
	$(CC) $^ -o $@ ${LIBUV_LDFLAGS} ${LIBUV_CFLAGS}

test-leak: test
	valgrind ./test

test-ffi: libpolluv.so
	luajit test.lua

clean:
	rm -rf zig-cache libpolluv.so obj test
