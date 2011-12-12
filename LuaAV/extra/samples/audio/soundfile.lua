
local Array = require "Array"
local soundfile = require "audio.soundfile"

local soundfilepath = LuaAV.findfile("flute.wav")
local soundfilepath = LuaAV.findfile("gong.wav")
--local soundfilepath = LuaAV.findfile("water3.wav")

local a = audio.soundfile.read(soundfilepath)

print("read file", soundfilepath, "into array", a)
print("channels:", a.components)
print("frames:", a.dim[1])

local dur = a.dim[1]/audio.samplerate()
print("dur:", dur)

while now() < dur do
	local frame = a:read(now() * audio.samplerate())
	print( unpack(frame) )
	wait(0.1)
end

