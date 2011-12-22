local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local app = require("app")

local Image = require("Image")

local Gui = require("gui.Context")
local Rect = require("gui.Rect")
local Slider = require("gui.Slider")
local Button = require("gui.Button")
local Label = require("gui.Label")
local Textfield = require("gui.Textfield")
-------------------------------------------------
local ctx = "Gui Starter"
win = Window{
	title = ctx, 
	origin = {0, 0}, 
	dim = {600, 480},
	mousemove = true,
}

local tex = Texture(ctx)
local circle = true

-- globals
local color = {1, 0, 0.5, 1}

-- create the gui
local gui = Gui{
	ctx = ctx,
	win = win,
	dim = win.dim,
}

-- create some widgets
local btnlabel = Label{
	rect = Rect(10, 10, 50, 15),
	label = "Shape:",
	--border = true,
	horizontal_align = Label.LEFT,
	vertical_align = Label.MIDDLE,
	text_color = {1, 1, 1, 1},
}
local gbtn = Button{
	rect = Rect(50, 10, 15, 15),
	value = circle,
	toggles = true,
}

local xslider = Slider{
	rect = Rect(10, 30, 100, 10),
	value = 0,
	range = {-1, 1},
}

-- add them to the gui
gui:add_view(gbtn)
gui:add_view(btnlabel)
gui:add_view(xslider)

function win:copy()
	gui:copy()
end

function win:cut()
	gui:cut()
end

function win:paste()
	gui:paste()
end

function win:selectall()
	gui:selectall()
end

-- register for notifications
gbtn:register("value", function(w)
	circle = w.value
end)

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


local
function draw_circle()
	gl.PointSize(20)
	gl.Begin(GL.POINTS)
		gl.Vertex(xslider.value, 0, 0)
	gl.End()
end

local
function draw_x()
	gl.Begin(GL.LINES)
		gl.Vertex(xslider.value-0.1, -0.1, 0)
		gl.Vertex(xslider.value+0.1, 0.1, 0)
		
		gl.Vertex(xslider.value-0.1, 0.1, 0)
		gl.Vertex(xslider.value+0.1, -0.1, 0)
	gl.End()
end

function win:draw()
	gui:draw()
	
	gl.Disable(GL.DEPTH_TEST)
	gl.Color(1, 1, 1, 1)
	tex:bind(0)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind(0)
	
	gl.Color(color)
	if(circle) then
		draw_circle()
	else
		draw_x()
	end
end