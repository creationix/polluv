local ffi = require 'ffi'

local function loadHeader(path)
    local file = io.open(path, "r")
    local header = string.gsub(file:read "*a", "\n#[^\n]*", "")
    file:close()
    return header
end

local polluv = ffi.load("./libpolluv.so")
ffi.cdef(loadHeader "uv.h")
ffi.cdef(loadHeader "polluv.h")

print("Current libuv version = " .. ffi.string(polluv.uv_version_string()))

local l = ffi.new("uv_event_loop_t")
print(l)
polluv.uv_event_init(l, polluv.uv_default_loop())

local timer = ffi.cast("uv_timer_t*", ffi.new("uint8_t[?]", polluv.uv_handle_size(polluv.UV_TIMER)))
print(timer)
polluv.uv_timer_init(l.loop, timer)
polluv.uv_timer_start(timer, polluv.uv_event_timer_cb, 100, 500)

local evt
repeat
    evt = polluv.uv_event_shift(l)
    print("Got event! " .. ffi.string(polluv.uv_event_type_name(evt.type)))
    if evt.type == polluv.UV_EVENT_TIMER_CB then
        local delay = polluv.uv_timer_get_repeat(evt.args.timer) - 100
        print("UV_EVENT_TIMER_CB delay=" .. tonumber(delay))
        if delay > 0 then
            polluv.uv_timer_set_repeat(evt.args.timer, delay)
        else
            polluv.uv_close(ffi.cast("uv_handle_t*", evt.args.timer), polluv.uv_event_close_cb)
        end
    else
        print("TODO: handle type " .. tonumber(evt.type))
    end

until evt.type == polluv.UV_EVENT_NONE
