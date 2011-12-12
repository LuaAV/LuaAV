LuaAV.addmodulepath(script.path.."/modules")
--------------------------------------------
local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")

local color = require("color")
local Label = require("Label")

local format = string.format
local pi = math.pi
local cos = math.cos
local sin = math.sin
local min = math.min
local max = math.max
local log = math.log
local floor = math.floor
local deg = math.deg
local rad = math.rad
local acos = math.acos
local abs = math.abs


local ctx  = "Named Colors"
win = Window(ctx, 0, 0, 720, 480)
win.clearcolor = {0.91, 0.91, 0.91, 0}


local keys = {
	[9] = 'tab',
	[27] = 'esc',
	[48] = '0',
	[49] = '1',
	[50] = '2',
	[108] = 'l',
	[113] = 'q',
	[114] = 'r',	
	[115] = 's',
}

function win:key(e, k)
	if(e == "down") then
		local k = keys[k] or k
		if(k == 'esc') then
			self.fullscreen = not self.fullscreen
		end
	end
end

local named_colors = {}
local sorted_colors = {}
for k, v in pairs(color) do
	if(type(v) == "table") then
		named_colors[k] = v
		sorted_colors[#sorted_colors+1] = v
	end
end

function huesort(colors)
	table.sort(colors, function(a, b)
		local ahsl = color.RGBtoHSL(a)
		local bhsl = color.RGBtoHSL(b)
		if(ahsl[1] == bhsl[1]) then
			if(ahsl[2] == bhsl[2]) then
				return ahsl[3] < bhsl[3]
			else
				return ahsl[2] < bhsl[2]
			end
		else
			return ahsl[1] < bhsl[1]
		end
	end)
end

function satsort(colors)
	table.sort(colors, function(a, b)
		local ahsl = color.RGBtoHSL(a)
		local bhsl = color.RGBtoHSL(b)
		if(ahsl[2] == bhsl[2]) then
			if(ahsl[1] == bhsl[1]) then
				return ahsl[3] < bhsl[3]
			else
				return ahsl[1] < bhsl[1]
			end
		else
			return ahsl[2] < bhsl[2]
		end
	end)
end

huesort(sorted_colors)
--satsort(sorted_colors)

function win:draw()
	gl.Disable(GL.DEPTH_TEST)
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	
	
	sketch.enter_ortho(-.01, 0, 1.02, 1)
	gl.Begin(GL.QUADS)
	local ncolors = #sorted_colors
	local ds = 1/ncolors
	for i=1, ncolors do
		local c = sorted_colors[i]
		gl.Color(c)
		sketch.quad((i-1)*ds, 0.3, ds, 0.4)
	end
	gl.End()
	sketch.leave_ortho()
end