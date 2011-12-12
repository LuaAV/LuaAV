
local assert, pairs, tostring, type = assert, pairs, tostring, type

local co = require "coroutine"
local resume = co.resume
local yield = co.yield

local midi = require "midi"


module 'midiutils'
----------------------------------------------------------

--- Transforms a stream of raw MIDI data into parameter do of the form PARAM = {PAR, VAL}
-- where PAR is the parameter ID and VAL is its current value
-- i.e. transforms the sequence {MIDI, MIDI, MIDI, ...} -> {PARAM, PARAM, ...}
-- @param midihandler  a function that loops over a sequence of MIDI messages
-- 						and yields PARAM data as it streams in
function collect(midihandler, input)
	local co_read = co.create(midihandler)

	return
	function()
		-- infinite loop over MIDI messages
		while(true) do
			local msg, ch, cmd, xy = input:readbytes()
			while(msg) do
				local status, par, val = resume(co_read, msg, ch, cmd, xy)
				if(par) then
					yield(par, val)
				end
				msg, ch, cmd, xy = input:readbytes()
			end	
			yield()
		end
	end
end

--- Polls a MIDI device and routes parameters to their destination.
-- Adapts {MIDI, MIDI, MIDI, ...} -> t[k] = PARAM.VAL
-- @param midihandler	a function that loops infinitely over MIDI messages and returns PARAMS
-- @paramhandler	a function that dispatches an incoming value given the parameter's PAR (id)
function poll(midihandler, paramhandler, input)
	input = input or midi.input()
	
	-- generate the MIDI -> PARAM loop
	local co_midi = co.create(collect(midihandler, input))

	-- function to poll currently buffered MIDI values
	return
	function()
		local status, par, val = resume(co_midi)
		while(par) do
			-- process params
			paramhandler(par, val)
			status, par, val = resume(co_midi)
		end
	end
end

--- Returns a handler function for an incoming parameter value.
-- Connects PARAMVAL -> t[k]
-- @param ref {table, key}
-- @param range {min, max}
-- @retval a function connecting a ranged value to a reference
function genhandler(ref, range)
	-- define local upvalues
	local t = ref[1]
	local k = ref[2]
	local scale = range[2] - range[1]
	local bias = range[1]
	
	-- generate handler function
	return
	function(val)
		t[k] = val*scale + bias
	end
end
