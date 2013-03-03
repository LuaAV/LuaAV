#!/usr/bin/luajit

-- this one has to be first:
local av = require "av"

local ffi = require "ffi"

local glfw = require "glfw"

local gl = require "gl"
assert(glfw.Init() ~= 0, "failed to initialize GLFW")

--glfw.SetErrorCallback(function(code, str) print("GLFW error", code, str) end)


local w, h = 1024, 768
local title = "untitled"

local window = glfw.OpenWindow(w, h, 0, 0, 0, 0, 0, 0, glfw.WINDOWED );
assert(window ~= 0)

--local mycb = ffi.cast("GLFWkeyfun", function(w, x, y) print("get", w, x, y) end)
--local mycb = ffi.cast("int (*)(int x)", function(x) print("get", x) end)
--print(mycb)

glfw.SetWindowSizeCallback(function(w1, h1)
	w, h = w1, h1
	print("resized", w, h)
end)

-- PANIC: unprotected error in call to Lua API (bad callback)
---[[
function closecb()	
	print("close")
	return 0
end
glfw.SetWindowCloseCallback(closecb)
--]]
glfw.SetKeyCallback(function(c, k)
	print("key", c, k)
end)

--[[
 void glfwSetKeyCallback( GLFWkeyfun cbfun );
 void glfwSetCharCallback( GLFWcharfun cbfun );
 void glfwSetMouseButtonCallback( GLFWmousebuttonfun cbfun );
 void glfwSetMousePosCallback( GLFWmouseposfun cbfun );
 void glfwSetMouseWheelCallback( GLFWmousewheelfun cbfun );
--]]
--glfw.SetKeyCallback(window, mycb)

--glfw.Enable(window, glfw.STICKY_KEYS)
glfw.SwapInterval(1)

local mx, my = ffi.new( "int[1]" ), ffi.new( "int[1]" )
local sw, sh = ffi.new( "int[1]" ), ffi.new( "int[1]" )

while true do
	
	gl.Viewport(0, 0, w, h)
	gl.ClearColor(0, 0.7, 0.2, 0)
	gl.Clear(gl.COLOR_BUFFER_BIT)
	gl.MatrixMode(gl.PROJECTION)
	gl.LoadIdentity()
	
	gl.MatrixMode( gl.MODELVIEW )
	gl.LoadIdentity()
	
	gl.Begin(gl.TRIANGLES)
	for i = 1, 10 do
		gl.Color3f(1, 0, 0)
		gl.Vertex3f(math.random(), math.random(), math.random())
	end
	gl.End() 
	--[[
	local seconds_since_init = glfw.GetTime()
	
	--glfw.GetMousePos(window, mx, my)
	--local mx, my = mx[0], my[0]
	
	--glfw.GetWindowSize(sw, sh)
	--local sw, sh = sw[0], sh[0]
	
	
	
	
	--]]
	glfw.SwapBuffers()
	--glfw.WaitEvents()
end

glfw.Terminate()

print("ok")