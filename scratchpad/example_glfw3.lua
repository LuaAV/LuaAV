#!/usr/bin/luajit

-- this one has to be first:
local av = require "av"

local ffi = require "ffi"

local gl = require "gl"
local glfw = require "glfw3"

assert(glfw.Init() ~= 0, "failed to initialize GLFW")
print(string.format("using %s", ffi.string(glfw.GetVersionString())))
glfw.SetErrorCallback(function(code, str) print("GLFW error", code, str) end)


local w, h = 1024, 768
local title = "untitled"
local window = glfw.CreateWindow(w or 0, h or 0, title or "", nil, nil)
assert(window ~= nil)


--local mycb = ffi.cast("GLFWkeyfun", function(w, x, y) print("get", w, x, y) end)
--local mycb = ffi.cast("int (*)(int x)", function(x) print("get", x) end)
--print(mycb)

--glfw.SetKeyCallback(window, mycb)

--glfw.Enable(window, glfw.STICKY_KEYS)
glfw.SwapInterval(1)

local mx, my = ffi.new( "int[1]" ), ffi.new( "int[1]" )
local sw, sh = ffi.new( "int[1]" ), ffi.new( "int[1]" )

while true do
	
	print("draw")
	
	local seconds_since_init = glfw.GetTime()
	
	--glfw.GetMousePos(window, mx, my)
	--local mx, my = mx[0], my[0]
	
	glfw.GetWindowSize(window, sw, sh)
	local sw, sh = sw[0], sh[0]
	
	gl.Viewport(0, 0, sw, sh)
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
	
	glfw.SwapBuffers(window)
	glfw.PollEvents()
	--glfw.WaitEvents()
end

glfw.Terminate()

print("ok")