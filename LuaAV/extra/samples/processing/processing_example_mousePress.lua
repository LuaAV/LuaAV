-- Click. 
-- 
-- Move the mouse to position the shape. 
-- Press the mouse button to invert the color. 
--
-- Note that we use isMousePressed instead of mousePressed as you can't have the same symbol be both an event
-- handler and a variable in Lua

require "processing"

function setup()
	size(200, 200)  -- Size must be the first statement
	fill(126)
	background(156)
end

function draw()
	--background(156)
	
	if(isMousePressed) then
		stroke(255)
	else
		stroke(0)
	end
	
	line(mouseX-66, mouseY, mouseX+66, mouseY)
	line(mouseX, mouseY-66, mouseX, mouseY+66)
end

