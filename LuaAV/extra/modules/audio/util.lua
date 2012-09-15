local math = math
local ipairs = ipairs
local tostring = tostring
local tonumber = tonumber
local print = print
local audio = require "audio"

--- Audio related utilities
-- @name audio.util
module(...)

local log = math.log
local log10 = math.log10
local round = math.round
local floor = math.floor
local LOG2 = log(2)
local pow = math.pow
local SR = audio.samplerate()

--- convert amplitude (0..1) to decibels:
-- @param amplitude (0..1)
-- @ret decibels
function atodb(amplitude)
	return 20*log10(amplitude)
end

--- convert amplitude (0..1) to decibels:
-- @param db decibels
-- @ret amplitude (0..1)
function dbtoa(db)
	return 10^(db/20)
end

--- convert MIDI note number to frequency
-- note: assumes middle A4 is freq 440Hz and MIDI note 69
-- @param notenumber MIDI note number
-- @ret frequency in Hertz
function mtof(notenumber)
	return 440*2^((notenumber-69)/12)
end

--- convert frequency to MIDI note number
-- note: assumes middle A4 is freq 440Hz and MIDI note 69
-- @param frequency in Hertz
-- @ret notenumber MIDI note number
function ftom(frequency)
	return 69 + (12/LOG2)*log(frequency/440)
end

-- convert MIDI note name to note number (and back)
local notenames = {'C ','C#','D ','D#','E ','F ','F#','G ','G#','A ','A#','B '}
for i, v in ipairs(notenames) do notenames[v] = i end
for i, v in ipairs(notenames) do notenames[v:lower()] = i end

--- convert note name to note number (chroma)
-- Valid names: C, C#, c, c#, etc.
-- @param n note number
-- @ret note name
function ntom(n)
	local name = n:sub(1, 2):upper()
	local note = notenames[name]
	if note then
		local number = (#n > 2) and floor(tonumber(n:sub(3))) or 0
		return number*12 + note - 1
	end
end

--- convert note number (chroma) to note name
-- Names formated as C, C# etc.
-- @param n note name
-- @ret note number
function mton(n)
	return notenames[floor(1+n%12)] .. tostring(floor(n/12))
end

-- returns multiplicaton factor for reaching -60 dB after a given time in seconds
-- @param s seconds
-- @ret multiplication factor
function t60(s)
	return pow(0.001, 1./(s * SR));
end

