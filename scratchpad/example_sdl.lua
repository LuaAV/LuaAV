-- this one has to be first:
local av = require "av"

local ffi = require "ffi"
ffi.cdef[[
	void Sleep(int ms);
	int poll(struct pollfd *fds, unsigned long nfds, int timeout);
]]
if ffi.os == "Windows" then
	function sleep(s)
		ffi.C.Sleep(s*1000)
	end
else
	function sleep(s)
		ffi.C.poll(nil, 0, s*1000)
	end
end

local gl = require "gl"
local sdl = require "sdl"

sdl.SDL_Init(0xFFFF)
sdl.GL_SetAttribute(sdl.GL_DOUBLEBUFFER, 1)

local screen
local fullscreen = false
local w, h = 1024, 480

function gofullscreen(fs)
	if fs then	
		screen = sdl.SetVideoMode(
			0, 0, 
			32, 
			bit.bor( sdl.FULLSCREEN, sdl.SDL_OPENGL)
		)
		fullscreen = true
	else
		w, h = 1024, 480
		screen = sdl.SDL_SetVideoMode( 
			w, h, 
			32,		-- bits per pixel
			bit.bor( sdl.SDL_RESIZABLE, sdl.SDL_OPENGL)
		)
		fullscreen = false
	end
	w, h = screen.w, screen.h
end

gofullscreen(false)

a = 0

assert(NULL ~= screen, ffi.string(sdl.SDL_GetError()))
local event = ffi.new( "SDL_Event" )

local run = true
while run do
	
	gl.Viewport(0, 0, w, h)
	gl.ClearColor(0, 0.1, 0.2, 0)
	gl.Clear(gl.COLOR_BUFFER_BIT)
	
	gl.MatrixMode(gl.PROJECTION)
	gl.LoadIdentity()
	
	gl.MatrixMode( gl.MODELVIEW )
	gl.LoadIdentity()
	
	gl.Begin(gl.LINE_STRIP)
	for i = 1, 30 do
		gl.Color3f(i/30, i/30, 0)
		gl.Vertex3f(math.sin((a+i) * 0.1), math.cos((a+i) * 0.1), 0)
	end
	gl.End() 
	a = a + 0.1
	
	
	sdl.GL_SwapBuffers()
	--sdl.SDL_Flip( screen )
	-- sleep some:
	--sleep(0.001)
	
	-- handle events:
	while sdl.SDL_PollEvent( event ) ~= 0 do
		if event.type == sdl.SDL_MOUSEMOTION or event.type == sdl.SDL_FINGERMOTION then
			--print("up")
		elseif event.type == sdl.SDL_MOUSEWHEEL then
			
			print("wheel")
		elseif event.type == sdl.SDL_MOUSEBUTTONDOWN or event.type == sdl.SDL_FINGERDOWN then				
			print("down")
		elseif event.type == sdl.SDL_MOUSEBUTTONUP or event.type == sdl.SDL_FINGERUP then
			print("up")
		elseif event.type == sdl.SDL_KEYDOWN then
			local ks = event.key.keysym
			local scancode = ks.scancode
			local unicode = ks.unicode
			local sym, mod = ks.sym, ks.mod
			if (sym == 113 and (mod == sdl.KMOD_LGUI or mod == sdl.KMOD_RGUI)) 
			or unicode == 2262565888 then
				run = false
				break
			elseif mod == 27 then
				gofullscreen(not fullscreen)
			else
				print("keydown", sym, mod, scancode, unicode) --, string.char(sym))			
			end
		elseif event.type == sdl.SDL_KEYUP then
			local ks = event.key.keysym
			local scancode = ks.scancode
			local unicode = ks.unicode
			local sym, mod = ks.sym, ks.mod
			print("keyup", sym, mod, scancode, unicode)
		elseif event.type == sdl.SDL_VIDEORESIZE then
			print("resize")
			local resize = event.resize
			w, h = resize.w, resize.h
			print(resize.w, resize.h)
		elseif event.type == sdl.SDL_QUIT then
			run = false
			break
		end
	end
end

-- force quit here to avoid the segfault when sdl.dylib gets unloaded
os.exit()