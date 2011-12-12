require "processing"


-- The statements in the setup() function 
-- execute once when the program begins
function setup()
--	size(500, 500)  -- Size must be the first statement
	stroke(255)     -- Set line drawing color to white
	frameRate(30)
end

y = 100

function mouseMoved()
   print("x = " .. mouseX) 
end
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
	point(5,5)
	strokeWeight(2)
	fill(50,0,255,1)
	rect(50, 50, 100, 100)
	fill(0,255,0,1)
	triangle(100,100,200,100,150,300) 
	fill(255,255,0,1)
	quad(25,25,50,25,55,45,30,45)
	
	strokeJoin(BEVEL)
	ellipseMode(CORNERS)
	arc(100, 100, 200, 200, 0, PI)
	
end

