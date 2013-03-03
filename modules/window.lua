local ffi = require 'ffi'
local lib = ffi.C

local ok, builtin = pcall(require, "builtin")
if not ok then
	error("running in native luajit is not yet supported")
end

local Window = {}

function Window:__tostring()
	return string.format("Window(%p)", self)
end

local key_events = {
	"down",
	"up"
}

local mouse_events = {
	[0] = "down",
	"up",
	"drag",
	"move",
}

local t = lib.av_time()

function Window:__newindex(k, v)
	if k == "key" then
		self.onkey:set(function(self, e, k)
			e = key_events[e]
			
			-- built-in keys:
			if e == "down" and k == 27 then
				-- fullscreen flip:
				self.fullscreen = not self.fullscreen
			end
		
			local ok, err = pcall(v, self, e, k)
			if not ok then print(debug.traceback(err)) end
		end)
		
	elseif k == "draw" then
		self.ondraw:set(function(self)		
			local t1 = lib.av_time()
			dt = t1 - t
			t = t1
			
			local ok, err = pcall(v, self, self.width, self.height, dt)
			if not ok then print(debug.traceback(err)) end	
		end)
		
	elseif k == "create" then
		self.oncreate:set(function(self)
			local ok, err = pcall(v, self)
			if not ok then print(debug.traceback(err)) end
		end)
		
	elseif k == "resize" then
		self.onresize:set(function(self, w, h)
			local ok, err = pcall(v, self, w, h)
			if not ok then print(debug.traceback(err)) end
		end)
		
	elseif k == "visible" then
		self.onvisible:set(function(self, s)
			local ok, err = pcall(v, self, s)
			if not ok then print(debug.traceback(err)) end
		end)
		
	elseif k == "mouse" then
		self.onmouse:set(function(self, e, b, x, y)
			local ok, err = pcall(v, self, mouse_events[e], b, x, y)
			if not ok then print(debug.traceback(err)) end
		end)
		
	elseif k == "fullscreen" then
		self:setfullscreen(v)
		
	elseif k == "title" then
		self:settitle(v)
		
	elseif k == "dim" then
		self:setdim(unpack(v))
		
	else
		error("cannot assign to Window: "..k)
	end
end

function Window:__index(k)
	if k == "fullscreen" then
		return self.is_fullscreen ~= 0
	elseif k == "dim" then
		return { self.width, self.height }
	else
		return Window[k]
	end
end

setmetatable(Window, {
	__index = function(self, k)
		Window[k] = lib["av_window_" .. k]
		return Window[k]
	end,
})

ffi.metatype("av_Window", Window)

local win = lib.av_window_create()

-- set default callbacks:
win.ondraw = function(self) end
win.oncreate = function(self) end
win.onkey = function(self, e, k) end
win.onmouse = function(self, e, b, x, y) end
win.onvisible = function(self, s) end
win.onresize = function(self, w, h) end

return win