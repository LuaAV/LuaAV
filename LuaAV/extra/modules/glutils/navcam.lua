local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local print = print

local math = require("math")
local table = require("table")
local co = require("coroutine")

local abs = math.abs
local acos = math.acos
local asin = math.asin
local sqrt = math.sqrt
local atan2 = math.atan2
local pi = math.pi

local gl = require("opengl")
local GL = gl
local glu = require("opengl.glu")
local Camera = require("opengl.Camera")

local space = require("space")
local vec3 = require("space.vec3")

local keys = {
	[97] = "a",
    [65] = "a",

	[98] = "b",
	[99] = "c",
	[100] = "d",
	[68] = "d",

	[101] = "e",
	[102] = "f",
	[103] = "g",
	[104] = "h",
	[105] = "i",
	[106] = "j",
	[107] = "k",
	[108] = "l",
	[109] = "m",
	[110] = "n",
	[111] = "o",
	[112] = "p",
	[113] = "q",
	[114] = "r",
	[115] = "s",
	[116] = "t",
	[117] = "u",
	[118] = "v",
	[119] = "w",
    [87] = "w",

	[120] = "x",
	[121] = "y",
	[122] = "z",
    [90] = "z",

	[63232] = "up",
	[16777235] = "up",

	[63233] = "down",
	[16777237] = "down",

	[63234] = "left",
	[16777234] = "left",

	[63235] = "right",
	[16777236] = "right",

	[96] = "home",
	
	[91] = "upshift",
	[47] = "downshift",
}

local
function key_event(f, rate)
	rate = rate or 1/33
	
	local ff = co.wrap(function()
		while(true) do
			local self, msg = co.yield()
			if(msg == "down") then
				go(function()
					while(msg == "down") do
						f(self)
						wait(rate)
					end
				end)
				while(msg == "down") do
					self, msg = co.yield()
				end
			end
		end
	end)
	
	ff()
	
	return ff
end

local moves = {
	left = function(self) self:movex(self.ds) end,
	right = function(self) self:movex(-self.ds) end,
	
	down = function(self) self:movez(-self.ds) end,
	up = function(self) self:movez(self.ds) end,
	
	downshift = function(self) self:movey(-self.ds) end,
	upshift = function(self) self:movey(self.ds) end,
	
	a = function(self) self:yaw(self.da) end,
	d = function(self) self:yaw(-self.da) end,
	
	w = function(self) self:pitch(-self.da) end,
	z = function(self) self:pitch(self.da) end,
	
	home = function(self)
		self.eye = {0, 0, 4}
		self.lookat = {0, 0, 0}
		self.up = {0, 1, 0}
	end,
}


local ds = 0.035
local da = 1.25

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('glutils.navcam', setconstructor)
----------------------------------------------------------
function C:__call(init)
	init = init or {}
	
	local m = Camera(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.mouse_mode = "translate"
	m.mouse_ds = 0.025
	m.res = init.res or 1/60
	m.ds = init.ds or ds
	m.da = init.da or da
	m.actions = {}
	
	m.moves = {}
	for k, v in pairs(moves) do
		m.moves[k] = key_event(v, m.res)
	end

	return m
end

function M:resize(win)
 	local dim = win.dim
 	self.aspect = dim[1]/dim[2]
end

function M:step()
	for _, v in pairs(self.actions) do
		v(self)
	end
	self:update()
end

function M:key(win, e, k)
	local name = keys[k]
	local f = moves[name]
	if(f) then
		if(e == "down") then
			self.actions[name] = f
		else
			self.actions[name] = nil
		end
	end
end


local
function pickray(x, y)
	local p1 = { glu.UnProject(x, y, 0.) }
	local p2 = { glu.UnProject(x, y, 1.) }
	
	return p1, p2
end

function M:picktheray(x, y)
   return pickray(x, y)
end

function M:drag(rs1, re1, rs2, re2)
	local r1 = vec3.sub(re1, rs1)
	local r2 = vec3.sub(re2, rs2)

	local p1 = vec3.scale(r1, self.mouse_ds)
	local p2 = vec3.scale(r2, self.mouse_ds)
	
	local delta = vec3.sub(p2, p1)
	
	self:track(delta[1])
	self:pedd(delta[2])
	self:dolly(delta[3])
end

local
function generate_mouse_handler()
	local self
	local f = co.wrap(function()
		local e, btn, x, y, dim
		while(true) do
			e, btn, x, y, dim = co.yield()
			if(e == "down") then
				if(self.mouse_mode == "translate") then
					local last_rs, last_re = pickray(x, y)
	
					e, btn, x, y, dim = co.yield()
					if(e == "drag") then
						local rs, re = pickray(x, y)
						self:drag(last_rs, last_re, rs, re)
						
						while(e ~= "up") do
							e, btn, x, y, dim = co.yield()
							last_rs, last_re = rs, re
							rs, re = pickray(x, y)
							self:drag(last_rs, last_re, rs, re)
						end
					end
				end			
			end
		end
	end)
	f()
	
	return
	function(_self, ...)
		self = _self
		return f(...)
	end
end

M.mouse_handler = generate_mouse_handler()

function M:mouse(win, e, btn, x, y)
	--print("mouse")
	self:mouse_handler(e, btn, x, y, win.dim)
end
