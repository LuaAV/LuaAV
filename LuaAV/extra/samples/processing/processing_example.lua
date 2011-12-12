require "processing"

-- The statements in the setup() function 
-- execute once when the program begins
function setup()
	size(200, 200)  -- Size must be the first statement
	stroke(255)     -- Set line drawing color to white
	frameRate(30)
end

y = 100

-- The statements in draw() are executed until the 
-- program is stopped. Each statement is executed in 
-- sequence and after the last line is read, the first 
-- line is executed again.
function draw() 
	background(0)   -- Set the background to black
	y = y - 1 
	if (y < 0) then
		y = height 
	end
	line(0, y, width, y)  
end

