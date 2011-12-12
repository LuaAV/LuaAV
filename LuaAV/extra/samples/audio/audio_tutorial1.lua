-- loading in the audio.Def module
local Def = require "audio.Def"
-- making local references to functions used
local SinOsc = Def.SinOsc
local Env = Def.Env
local P = Def.P

-- constructing expressions from expression generators:
local osc = SinOsc{ freq = 440 }
local env = Env{ dur = 1 }
-- using math operators to create compound expressions:
local expr = osc * env * 0.25
print(expr) --> Expr(Mul)

-- calling Def to JIT compile this expression into a synthesis definition:
local mysound = Def { expr }
-- returns a constructor:
print(mysound) --> function

-- creating voices from this synthesis definition constructor:
for i = 1, 4 do
	-- launch a voice:
	mysound()
	-- wait some time before launching the next one:
	wait(1/i)
end


-- Adding parameters to the definition:
local mysound2 = Def{
	-- the parameter defaults:
	amp = 0.25, 
	dur = 1,
	freq = 440,
	-- the compound expression object:
	SinOsc{ freq = P"freq" } * Env{ dur = P"dur" } * P"amp"
}

-- using these parameters in voices:
for i = 1, 4 do
	-- launch a voice:
	mysound2{ freq = i*330, dur = 1/i }
	-- wait some time before launching the next one:
	wait(0.5)
end


-- the constructor returns a voice object (a proxy to the object in the audio engine)
local voice = mysound2{}
print(voice) --> Synth

for i = 1, 10 do
	-- parameters of a voice can be set:
	voice.freq = math.random(10) * 110
	wait(0.1)
end

-- parameters can also be read:
print(voice.freq) --> 660


-- Using the stop method
local mysound3 = Def{
	amp = 0.25, freq = 440,
	-- the compound expression object (Note: has no Env):
	SinOsc{ freq = P"freq" } * P"amp"
}

local voice = mysound3{ freq = 220 }
wait(1)

-- make it stop!
voice:stop()


