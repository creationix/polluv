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

-- Generate "concrete" definitions for the structs so we know their size at least.
ffi.cdef(string.format("struct uv_loop_s { uint8_t _[%d]; }", tonumber(polluv.uv_loop_size())))
ffi.cdef(string.format("struct uv_tcp_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_TCP))))
ffi.cdef(string.format("struct uv_udp_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_UDP))))
ffi.cdef(string.format("struct uv_pipe_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_NAMED_PIPE))))
ffi.cdef(string.format("struct uv_tty_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_TTY))))
ffi.cdef(string.format("struct uv_poll_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_POLL))))
ffi.cdef(string.format("struct uv_timer_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_TIMER))))
ffi.cdef(string.format("struct uv_prepare_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_PREPARE))))
ffi.cdef(string.format("struct uv_check_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_CHECK))))
ffi.cdef(string.format("struct uv_idle_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_IDLE))))
ffi.cdef(string.format("struct uv_async_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_ASYNC))))
ffi.cdef(string.format("struct uv_process_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_PROCESS))))
ffi.cdef(string.format("struct uv_fs_event_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_FS_EVENT))))
ffi.cdef(string.format("struct uv_fs_poll_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_FS_POLL))))
ffi.cdef(string.format("struct uv_signal_s { uint8_t _[%d]; }", tonumber(polluv.uv_handle_size(polluv.UV_SIGNAL))))

print("Current libuv version = " .. ffi.string(polluv.uv_version_string()))

local Polluv = {}
function Polluv:init(loop) return polluv.polluv_init(self, loop) end
function Polluv:close() return polluv.polluv_close(self) end
function Polluv:shift() return polluv.polluv_shift(self) end
function Polluv:iterate()
    return function ()
        while true do
            local evt = self:shift()
            if evt.type == polluv.POLLUV_NONE then return end
            local typeName = ffi.string(polluv.polluv_type_name(evt.type))
            p(typeName, Polluv.handlers[typeName])
            local args = {}
            local handler
            if typeName == "CLOSE_CB" then
                args[1] = evt.args.handle:toType()
                local addr = evt.args.handle:addr()
                handler = Polluv.handlers.CLOSE_CB[addr]
                Polluv.handlers.CLOSE_CB[addr] = nil
            elseif typeName == "TIMER_CB" then
                args[1] = evt.args.timer
                local addr = evt.args.timer:addr()
                handler = Polluv.handlers.TIMER_CB[addr]
            end
            p(unpack(args))
            if (handler) then
                handler(unpack(args))
            else
                return typeName, args
            end
        end
    end
end
Polluv.handlers = {
    CLOSE_CB = {},
    TIMER_CB = {},
}
Polluv.new = ffi.metatype("polluv_state_t", {__index=Polluv})

local Event = {}
ffi.metatype("polluv_event_t", {__index=Event})

local Loop = {}
function Loop.default() return polluv.uv_default_loop() end
function Loop:init() return polluv.uv_loop_init(self) end
function Loop:close() return polluv.uv_loop_close(self) end
function Loop:alive() return polluv.uv_loop_alive(self) end
function Loop:run(mode) return polluv.uv_run(self, mode) end
function Loop:stop() return polluv.uv_loop_stop(self) end
Loop.new = ffi.metatype("uv_loop_t", {__index=Loop})

local Handle = {}
function Handle:close(callback)
    self:on("CLOSE_CB", callback)
    return polluv.uv_close(ffi.cast("uv_handle_t*", self), polluv.polluv_close_cb)
end
function Handle:getLoop() return polluv.uv_handle_get_loop(ffi.cast("uv_handle_t*", self)) end
function Handle:getType() return polluv.uv_handle_get_type(ffi.cast("uv_handle_t*", self)) end
function Handle:getTypeName() return ffi.string(polluv.uv_handle_type_name(self:getType())) end
function Handle:getData() return polluv.uv_handle_get_data(ffi.cast("uv_handle_t*", self)) end
function Handle:setData(data) return polluv.uv_handle_set_data(ffi.cast("uv_handle_t*", self), data) end
function Handle:isActive() return polluv.uv_is_active(ffi.cast("uv_handle_t*", self)) ~= 0 end
function Handle:addr() return tonumber(ffi.cast('intptr_t', ffi.cast('void *', self))) end
function Handle:toType()
    local typeName = self:getTypeName()
    return ffi.cast(({
        timer = "uv_timer_t*"
    })[typeName], self)
end
function Handle:on(event, handler)
    Polluv.handlers[event][self:addr()] = handler
end
ffi.metatype("uv_handle_t", {__index=Handle})

local Timer = setmetatable({}, {__index=Handle})
function Timer:init(loop) return polluv.uv_timer_init(loop, self) end
function Timer:start(timeout, _repeat, callback)
    self:on("TIMER_CB", callback)
    return polluv.uv_timer_start(self, polluv.polluv_timer_cb, timeout, _repeat)
end
function Timer:stop() return polluv.uv_timer_stop(self) end
function Timer:again() return polluv.uv_timer_again(self) end
function Timer:setRepeat(_repeat) return polluv.uv_timer_set_repeat(self, _repeat) end
function Timer:getRepeat() return polluv.uv_timer_get_repeat(self) end
function Timer:close(callback)
    self:on("TIMER_CB", nil)
    return Handle.close(self, callback)
end
Timer.new = ffi.metatype("uv_timer_t", {__index=Timer})

function main(polluv)

    local loop = Loop.new()
    loop:init()

    local state = Polluv.new()
    state:init(loop)

    local timer = Timer.new()
    timer:init(loop)

    timer:start(100, 500, function (self)
        local delay = self:getRepeat() - 100
        p{self,delay=delay}
        if delay > 0 then
            self:setRepeat(delay)
        else
            self:close(function (self)
                print("Timer is now closed!")
            end)
        end
    end)

    for typeName, args in state:iterate() do
        p{typeName, unpack(args)}
    end

end
main()
