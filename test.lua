local ffi = require 'ffi'

local function cinclude(lib, header)
    local l = ffi.load(lib)
    local file = open(header, "r")
    l.cdef(file:read "*a")
    file:close()
    return l
end

local polluv = cinclude("./main", "main.h")
p(polluv)
