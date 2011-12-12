local gl = require("opengl")
local GL = gl

local Gui = require("gui.context")
local Rect = require("gui.rect")
local Slider = require("gui.slider")
local Button = require("gui.button")
local Label = require("gui.label")
-------------------------------------------------
local ctx = "Gui Starter"
win = Window{
	title = ctx, 
	origin = {0, 0}, 
	dim = {600, 480},
	mousemove = true,
}

local gui = Gui{
	ctx = ctx,
	dim = win.dim,
}

---------------------------------------
-- create and add widgets here

---------------------------------------


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
	gui:key(e, k)
end

function win:mouse(e, btn, x, y, nclk)
	gui:mouse(e, btn, x, y, nclk)
end

function win:resize()
	gui:resize(self.dim)
end

function win:modifiers()
	gui:modifiers(self)
end

function win:draw()
	gui:draw()
	
	---------------------------------------
	-- draw graphics here
	
	---------------------------------------
end