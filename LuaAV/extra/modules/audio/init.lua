--- Audio in LuaAV
-- @name audio
module(...)

--- set/get the audio thread latency setting. 
-- this will affect how responsive LuaAV's audio synthesis is. A lower value will improve immediacy, but is more likely to suffer underruns that may compromise sample-accurate synthesis and scheduling. Default is 0.05 seconds (50ms).
-- @param seconds latency to set; if not given, latency() serves as a getter
-- @ret current latency setting
function latency(seconds) end

--- get the audio samplerate
-- @ret samplerate
function samplerate() end
