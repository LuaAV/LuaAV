local assert, pairs, tostring, type = assert, pairs, tostring, type
local print = print
local ipairs = ipairs
local setmetatable = setmetatable
local getfenv = getfenv
local unpack = unpack
local setfenv = setfenv
----------------------------------------------------------
local gl = require("opengl")
local GL = gl
----------------------------------------------------------
if(LuaAV.platform ~= "OSX") then
	require("lcairo")
end
local cairo = cairo
local CAIRO = cairo
----------------------------------------------------------
local cgl = (LuaAV.platform == "OSX") and require("cgl.canvas") or require("cgl")
----------------------------------------------------------
local table = require("table")
local string = require("string")
local format = string.format
----------------------------------------------------------
local app_utils = require("app.utilities")
----------------------------------------------------------
--- Utility functions for Cairo OpenGL functions
-- @name cgl.util
module(...)
----------------------------------------------------------
local
function getextension(filename)
	return filename:match(".+%.(%w+)$")
end

--- Convert an OpenGL function to one using the cgl backend
-- @param filename The file to save to
-- @param win The window (must have a a dim and clearcolor field)
-- @param draw The drawing function
function opengl_to_cgl(filename, win, draw, ENV)
	local path = app_utils.stripfilename(filename)
	app_utils.ensurepath(path)

	local ext = getextension(filename)
	local w = win.dim[1]
	local h = win.dim[2]
	
	local CGL = cgl
	local c
	if(ext == "pdf") then
		c = cgl(filename, w, h)
	else
		c = cgl(CAIRO.FORMAT_RGB24, w, h)
	end

	local ENV_gl = ENV.gl
	ENV.gl = c
	
	-- clear surface
	c.ClearColor(win.clearcolor)
	c.Clear()
	
	-- init camera
	c.MatrixMode(CGL.PROJECTION)
		c.LoadIdentity()
		c.Perspective(45, w/h, 0.1, 100)
	
	c.MatrixMode(CGL.MODELVIEW)
		c.LoadIdentity()
		c.LookAt({0, 0, 4}, {0, 0, 0}, {0, 1, 0})
	
	-- render
	draw()
	
	-- swap gl back in
--	setfenv(draw, gl_env)
	ENV.gl = ENV_gl
	
	if(ext == "pdf") then
		c.Save()
	else
		c.Save(filename)
	end
end

local
function enter_cairo(src, dst)
	local repl = {}
	for k, v in pairs(src) do
		local ty = type(v)
		if(ty == "function") then
			repl[k] = v
			src[k] = dst[k]
		elseif(ty == "table" and dst[k]) then
			repl[k] = enter_cairo(v, dst[k])
		end
	end
	return repl
end

local
function leave_cairo(src, repl)
	for k, v in pairs(repl) do
		local ty = type(v)
		if(ty == "function") then
			src[k] = v
		elseif(ty == "table") then
			leave_cairo(src[k], v)
		end
	end
end

function opengl_to_cgl_replace(filename, win, draw)
	local path = app_utils.stripfilename(filename)
	app_utils.ensurepath(path)

	local ext = getextension(filename)
	local w = win.dim[1]
	local h = win.dim[2]
	
	local CGL = cgl
	local c
	if(ext == "pdf") then
		c = cgl(filename, w, h)
	else
		c = cgl(CAIRO.FORMAT_RGB24, w, h)
	end
	
	local repl = enter_cairo(gl, c)
	
	-- clear surface
	c.ClearColor(win.clearcolor)
	c.Clear()
	
	-- init camera
	c.MatrixMode(CGL.PROJECTION)
		c.LoadIdentity()
		c.Perspective(45, w/h, 0.1, 100)
	
	c.MatrixMode(CGL.MODELVIEW)
		c.LoadIdentity()
		c.LookAt({0, 0, 4}, {0, 0, 0}, {0, 1, 0})
	
	-- render
	draw(win)
	
	leave_cairo(gl, repl)
	
	if(ext == "pdf") then
		c.Save()
	else
		c.Save(filename)
	end
end