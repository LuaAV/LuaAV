require "csound"

module(..., package.seeall)

function create(src)
	print("creating csound")

	local filename = os.tmpname()..".csd"
	local file = io.open(filename, "w")
	file:write(src)
	file:close()
	
	local c = csound.create("-d", filename) 
	assert(c, "csound create error")
	return c
end
