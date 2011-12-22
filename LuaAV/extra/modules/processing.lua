local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Draw = require("opengl.Draw")
local Window = Window

local color = require("color")
local RGBtoHSL = color.RGBtoHSL
local HSLtoRGB = color.HSLtoRGB

local unpack = unpack
local print = print
local pairs, ipairs = pairs, ipairs
local setmetatable = setmetatable
local rawget, rawset = rawget, rawset
local type = type
local tonumber, tostring = tonumber, tostring
local math, string = math, string

local _G = _G

module("processing")

local ctx = "LuaAV meets processing"

-- shift to center of pixel for sharp rendering
local fudge = 0.5

-- make window:
local win = Window{
	title = ctx,
	dim = {100, 100},
	mousemove = true,
	autoclear = false,
	clearcolor = { 0.5, 0.5, 0.5, 1 },
}
_G.width, _G.height = 100, 100

function size(w, h) 
	_G.width, _G.height = w, h
	win.dim = { w, h } 
end

function frameRate(fps) 
	win.fps = fps 
end

-- Draw state
local draw = Draw()
draw.colorRange = {255, 255, 255, 1}
draw.strokeWeight = 1
draw.strokeColor = { 0, 0, 0, 1 }

local doLoop = true

_G.pmouseX = 0
_G.pmouseY = 0
_G.mouseX = 0
_G.mouseY = 0
_G.PI = math.pi
_G.TWO_PI = 2*math.pi
_G.HALF_PI = 0.5*math.pi
_G.QUARTER_PI = 0.25*math.pi
for k, v in pairs(math) do
	_G[k] = v
	if type(v) == "number" then _G[k:upper()] = v end
end

_G.CENTER = Draw.CENTER
_G.RADIUS = Draw.RADIUS
_G.CORNER = Draw.CORNER
_G.CORNERS = Draw.CORNERS
_G.SQUARE = Draw.SQUARE
_G.PROJECT = Draw.PROJECT
_G.ROUND = Draw.ROUND
_G.MITER = Draw.MITER
_G.BEVEL = Draw.BEVEL
_G.RGB = Draw.RGB
_G.HSB = Draw.HSB
_G.POINTS = Draw.POINTS
_G.LINES = Draw.LINES
_G.TRIANGLES = Draw.TRIANGLES
_G.TRIANGLE_STRIP = Draw.TRIANGLE_STRIP
_G.TRIANGLE_FAN = Draw.TRIANGLE_FAN
_G.QUADS = Draw.QUADS
_G.CLOSE = Draw.CLOSE

function tocolor(r, g, b, a)
	return { r/255, g and g/255 or r/255, b and b/255 or r/255, a and a/255 or 1 }
end

-----------------------------------------------
-- Color
---------------------------------
--- Setting


function background(r, g, b, a)
	win.clearcolor = tocolor(r, g, b, a)
	gl.ClearColor(win.clearcolor)
	gl.Clear(GL.COLOR_BUFFER_BIT)
end

function colorMode(mode, ...)
	draw.colorMode = mode
	
	local range = {...}
	if(#range == 1) then
		range[2] = range[1]
		range[3] = range[1]
		range[4] = range[1]
		draw.colorRange = range
	else
		-- what about HSB?
		draw.colorRange = range
	end
end

function fill(r, g, b, a)
	draw.fillColor = {r, g, b, a}
	draw.fill = true
end

function noFill()
    draw.fill = false
end

function noStroke()
	draw.stroke = false
end

function stroke(...)
	draw.strokeColor = color(...) or draw.strokeColor
	draw.stroke = true
end

---------------------------------
--- Color
function alpha(c)
	return c[4]
end

function blendColor(c1, c2, mode)
	error("not yet implemented")
end

function blue(c)
	return c[3]
end

function brightness(c)
	return RGBtoHSL(c)[3]
end

local
function colorFromHex(str)
	local v = {}
	if #str <= 4 then
		-- e.g. "#FC0" -> FFCC00
		for c in str:gmatch("[%a%d]", "%1%1") do
			local b = tonumber(c, 16)
			v[#v+1] = (b*16)+b
		end
	else
		-- e.g. "#FFCC00"
		for c in str:gmatch("[%a%d][%a%d]") do
			v[#v+1] = tonumber(c, 16)
		end
	end
	return v
end

function color(...)
	
	local v = {...}
	if(#v == 0) then
		return			-- return nil to signify no color change
	elseif(#v == 1) then
		if type(v[1]) == "string" then
			v = colorFromHex(v[1])
		else
			v[1] = v[1]
			v[2] = v[1]
			v[3] = v[1]
			v[4] = 1
		end
	elseif(#v == 2) then
		v[1] = v[1]
		v[2] = v[1]
		v[3] = v[1]
		v[4] = v[2]
	elseif(#v == 3) then
		v[1] = v[1]
		v[2] = v[2]
		v[3] = v[3]
		v[4] = 1
	else
		v[1] = v[1]
		v[2] = v[2]
		v[3] = v[3]
		v[4] = v[4]
	end
	
	return v
end

function green(c)
	return c[2]
end

function hue(c)
	return RGBtoHSL(c)[1]
end

function lerpColor(c1, c2, amt)
	-- 
	error("TODO: lerpColor")
end

function red(c)
	return c[1]
end

function saturation(c)
	return RGBtoHSL(c)[2]
end

-----------------------------------------------
-- Math
---------------------------------
local pi = math.pi
function radians(v) return pi * v / 180 end
function degrees(v) return 180 * v / pi end

function random(a, b)
	a = a or 1
	if b then
		return a+math.random()*(b-a)
	else
		return math.random()*a
	end
end

function randomSeed(v) math.randomseed(v) end

-- TODO: noise() (Perlin noise generator)

function norm(v, low, high)
	return (v - low)/(high - low)
end
function map(v, low1, high1, low2, high2)
	return low2 + norm(v, low1, high1)*(high2 - low2)
end
function constrain(v, min, max)
	return math.min(max, math.max(min, v))
end
function lerp(v1, v2, amt)
	return v1 + amt*(v2 - v1)
end
function mag(a, b, c)
	c = c or 1
	return sqrt(a*a + b*b + c*c)
end
function dist(a1, a2, a3, a4, a5, a6)
	if a6 then
		local a, b, c = (a4 - a1), (a5 - a2), (a6 - a3)
		return sqrt(a*a + b*b + c*c)
	else
		local a, b = (a3 - a1), (a4 - a2)
		return sqrt(a*a + b*b)
	end
end

function int(v) return math.floor(v) end

-- TODO: bitwise operators are not supported.

-----------------------------------------------
-- Shape
---------------------------------
--- 2D Primitives
function arc(x, y, width, height, start, stop)
	draw:arc({x+fudge, y+fudge}, {width, height}, start, stop)
end

function ellipse(x, y, width, height)
	draw:ellipse({x+fudge, y+fudge}, {width, height})
end

function line(x0, y0, x1, y1)
	draw:line({x0+fudge, y0+fudge}, {x1+fudge, y1+fudge})
end

function point(x,y)
	 draw:point{x+fudge, y+fudge}
end

function quad(x0, y0, x1, y1, x2, y2, x3, y3)
    draw:quad({x0+fudge, y0+fudge}, {x1+fudge, y1+fudge}, {x2+fudge, y2+fudge}, {x3+fudge, y3+fudge})
end

function rect(x, y, width, height)
    draw:rect({x+fudge, y+fudge}, {width, height})
end

function triangle(x0, y0, x1, y1, x2, y2)
 	draw:triangle({x0+fudge, y0+fudge}, {x1+fudge, y1+fudge}, {x2+fudge, y2+fudge})
end

---------------------------------
--- Curves
function bezier(x1, y1, cx1, cy1, cx2, cy2, x2, y2)
	draw:bezier({x1+fudge, y1+fudge}, {cx1, cy1}, {cx2, cy2}, {x2+fudge, y2+fudge})
end

function bezierDetail(n)
	draw.bezierDetail = n
end

function bezierPoint(a, b, c, d, t)
	return draw:bezierPoint({a, 0}, {b, 0}, {c, 0}, {d, 0}, t)[1]
end

function bezierTangent(a, b, c, d, t)
	return draw:bezierTangent({a, 0}, {b, 0}, {c, 0}, {d, 0}, t)[1]
end

function curve(x1, y1, x2, y2, x3, y3, x4, y4)
	draw:curve({x1+fudge, y1+fudge}, {x2+fudge, y2+fudge}, {x3+fudge, y3+fudge}, {x4+fudge, y4+fudge})
end

function curverDetail(n)
	draw.curverDetail = n
end

function curvePoint(a, b, c, d, t)
	return draw:curvePoint({a, 0}, {b, 0}, {c, 0}, {d, 0}, t)[1]
end

function curveTangent(a, b, c, d, t)
	return draw:curveTangent({a, 0}, {b, 0}, {c, 0}, {d, 0}, t)[1]
end

function curveTightness(v)
	draw.curveTightness = v
end

---------------------------------
--- Attributes
function ellipseMode(mode)
	draw.ellipseMode = mode
end

function noSmooth()
	-- ignore
end

function rectMode(mode)
	draw.rectMode = mode
end

function smooth()
	-- ignore
end

function strokeCap(cap)
	draw.strokeCap = cap
end

function strokeJoin(join)
   	draw.strokeJoin = join
end

function strokeWeight(weight)
   	draw.strokeWeight = weight
end


---------------------------------
--- Vertex
-- only nil argument is implemented right now
function beginShape(mode)
	draw:beginShape(mode)
end

function endShape()
	draw:endShape()
end

function bezierVertex(cx1, cy1, cx2, cy2, x2, y2)
	draw:bezierVertex({cx1, cy1}, {cx2, cy2}, {x2+fudge, y2+fudge})
end

function curveVertex(x, y)
	draw:curveVertex({x+fudge, y+fudge})
end

--[[ ??
function beginShape(close)
	draw:endShape(close)
end
--]]

function vertex(x, y)
	draw:vertex{x+fudge, y+fudge}
end


function loop()
	doLoop = true
end

function noLoop()
	doLoop = false
end



function win:mouse(e, b, x, y)
	_G.pmouseX = _G.mouseX
	_G.pmouseY = _G.mouseY
	_G.mouseX = x
	_G.mouseY = y
	if b=="left" then _G.mouseButton = "LEFT" 
	elseif b=="right" then _G.mouseButton = "RIGHT" end
	if e=="move" then
		_G.isMousePressed = false
		if _G.mouseMoved then _G.mouseMoved() end
	elseif e=="down" then
		_G.isMousePressed = true
		if _G.mousePressed then _G.mousePressed() end
	elseif e=="up" then
		_G.isMousePressed = false
		if _G.mouseReleased then _G.mouseReleased() end	
		if _G.mouseClicked then _G.mouseClicked() end	
	elseif e=="drag" then
		_G.isMousePressed = true
		if _G.mouseDragged then _G.mouseDragged() end
	else
		--print(e, k, x, y)
	end
end

function win:key(e, k)
	if e=="down" then
		_G.isKeyPressed = true
		_G.keyCode = k
		_G.key = string.char(k)
		if _G.keyPressed then _G.keyPressed() end
	else
		_G.isKeyPressed = false
		_G.keyCode = k
		_G.key = string.char(k)
		if _G.keyReleased then _G.keyReleased() end
		-- TODO: this doesn't quite match Processing:
		if _G.keyTyped then _G.keyTyped() end	
	end
end

-- should this be win:init() instead?
function win:create()
	_G.width, _G.height = unpack(self.dim)
	-- clear window on resize:
	gl.Clear(GL.DEPTH_BUFFER_BIT)
	gl.Clear(GL.COLOR_BUFFER_BIT)
	gl.Disable(GL.DEPTH_TEST)
	sketch.enter_ortho(width, height)
	
	if _G.setup then 
		_G.setup() 
	end
	
	sketch.leave_ortho()
end

function redraw()
	local width, height = unpack(win.dim)
		
	gl.Disable(GL.DEPTH_TEST)
	
	-- set up 2D ortho mode:
	sketch.enter_ortho(width, height)

	if _G.draw then _G.draw() end
	
	-- done
	sketch.leave_ortho()
end

function win:resize()
	_G.width, _G.height = unpack(self.dim)
	
	if not doLoop then
		-- trigger draw here
		gl.Clear(GL.DEPTH_BUFFER_BIT)
		gl.Clear(GL.COLOR_BUFFER_BIT)
		redraw()
	end
end

function win:draw()
	if _G.draw and doLoop then 
		redraw(self)
	end
end


-- make module global:
for k, v in pairs(_M) do
	if k:sub(1,1) ~= "_" then 
		--print("api", k, v)
		_G[k] = v 
	end
end
