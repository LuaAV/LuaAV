-- Click. 
-- 
-- Move the mouse to position the shape. 
-- Press the mouse button to invert the color. 

require "processing"

local gl = require("opengl")
local GL = gl

function setup()
	size(200, 200)  -- Size must be the first statement
	fill(126)
	background(156)
end

--strokeCap(ROUND)
--strokeWeight(10)

function draw()
	if(mousePressed) then
		stroke(255)
	else
		stroke(0)
	end
	--line(mouseX-66, mouseY, mouseX+66, mouseY)
	--line(mouseX, mouseY-66, mouseX, mouseY+66)
	ellipse(50,50,80,80)
end

