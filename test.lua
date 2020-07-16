local ffi = require 'ffi'

local function cinclude(lib, header)
    local l = ffi.load(lib)
    local file = io.open(header, "r")
    ffi.cdef(file:read "*a")
    file:close()
    return l
end

local polluv = cinclude("./libpolluv.so", "polluv.h")
print(polluv)
