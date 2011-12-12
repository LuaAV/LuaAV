local setmetatable = setmetatable
local co = require 'coroutine'
local yield = co.yield

module 'midiutils.BCR2000'
----------------------------------------------------------

--- Transforms a raw MIDI stream from the BCR2000 controller 
-- in absolute 14-bit mode into PARAM data
-- @retval a function to process an ABS14 MIDI stream
function ABS14()
	-- set of every numerical value except {98, 6, 38}
	local basic_map = {
		-- explicit values in set (for fast detection)
		[99] = true,
		[1] = true,
		-- implicit values in set
		__index = function() return true end,
		
		-- explicit values not in set
		[98] = false,
		[6] = false,
		[38] = false,
	}
	-- enable implicit set members via metamethods
	setmetatable(basic_map, basic_map)
	
	
	return
	function(v1, v2, v3, v4)
		local msg, ch, cmd, xx = v1, v2, v3, v4
	
		local par
		local v_hi
		local v_lo
	
		-- midi message handling loop (for ABS14 parameters)
		-- state machine for pulling complete messages together
		while(true) do
			if(basic_map[cmd]) then
				msg, ch, cmd, xx = yield()
			elseif(cmd == 98) then
				par = xx
				msg, ch, cmd, xx = yield()
			elseif(cmd == 6) then
				v_hi = xx
				msg, ch, cmd, xx = yield()
			elseif(cmd == 38) then
				v_lo = xx
				local vv = v_hi*128 + v_lo
				msg, ch, cmd, xx = yield(par, vv)
			end
		end
	end
end
