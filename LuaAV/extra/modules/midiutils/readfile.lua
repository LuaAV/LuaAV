module ("readfile", package.seall)

local
function bit32(a, b, c, d) return a * 2^24 + b * 2^16 + c * 2^8 + d end

local
function readdelta(f)
	local value = 0
	local c = string.byte(f:read(1))
	while c and (c % 128) ~= 0 do -- end marker
		print(c, 0x80)
		value = value * 2^7 + c
		c = string.byte(f:read(1))
	end
	return value
end

function read(filename)
	local f = io.open(LuaAV.findfile(filename), "rb")
	
	-- ensure correct header:
	local tag = f:read(4)
	assert(tag == "MThd", "file has bad header")
	assert(string.byte(f:read(1)) == 0, "file has bad header")
	assert(string.byte(f:read(1)) == 0, "file has bad header")
	assert(string.byte(f:read(1)) == 0, "file has bad header")
	assert(string.byte(f:read(1)) == 6, "file has bad header")
	local format = (string.byte(f:read(2), 2, 2))
	local tracks = (string.byte(f:read(2), 2, 2))
	local timecode = { string.byte(f:read(2), 1, 2) }
	print(string.format("format %u tracks %u timecode %x %x", format, tracks, unpack(timecode)))
	
	for i = 1, tracks do
		print("track", i)
		
		-- assert validity:
		local tag = f:read(4)
		assert(tag == "MTrk", "file has bad track")
		-- get track len:
		local len = bit32(string.byte(f:read(4), 1, 4))
		print("len", len)
		
		-- read track data:
		while true do
			-- figure out the event delta:
			print("delta", readdelta(f))
			
			local data = f:read(1)	-- read one byte from the file
			if not data then break end
			local status = string.byte(data) -- convert to byte
			if status == 0x47 then break end -- track end marker
			
			
			if status == 0xFF then 
				print'meta' 
				-- TODO read more data
			end
			
			
			
		end
	end
	
	f:close()
end

--[[ --test
for i = 1, 5 do
	readmidi("example" .. tostring(i) .. ".mid")
end
]]