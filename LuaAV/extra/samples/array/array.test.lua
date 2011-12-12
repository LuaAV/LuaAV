local Array = require("Array")

local types = {
	[Array.Float32] = "Float32",
	[Array.Float64] = "Float64",
	
	[Array.SInt8] = "SInt8",
	[Array.SInt16] = "SInt16",
	[Array.SInt32] = "SInt32",
	[Array.SInt64] = "SInt64",
	
	[Array.UInt8] = "UInt8",
	[Array.UInt16] = "UInt16",
	[Array.UInt32] = "UInt32",
	[Array.UInt64] = "UInt64",
}

function print_array(x)
	print("------------------------------")
	print("components: "..x.components)
	print("type:       "..types[x.type])
	print("dim:        "..table.concat(x.dim, " "))
end

local a = Array()
print_array(a)

a.components = 3
print_array(a)

a.dim = {10, 10}
print_array(a)

a.type = Array.Float64
print_array(a)

print("******************************")
print_array(a.header)
--]]