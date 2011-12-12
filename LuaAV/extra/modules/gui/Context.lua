local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local ipairs = ipairs
local print = print

local co = coroutine
local table = table
-------------------------------------
local g = require("gui.draw")
local rect = require("gui.Rect")
local Layer = require("gui.Layer")
-------------------------------------
local app = require("app")
-------------------------------------
local Label = require("Label")
local font_path = LuaAV.findfile("VeraMono.ttf")
-------------------------------------

local Window = Window

--- Gui context for all interaction with widgets
-- The context holds all widgets and manages all interaction.  
-- Events from the containing window or view are piped in through 
-- the context and filter down to the appropriate widget.
-- @name gui.Context
module(...)

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

-- Cursor styles
M.ARROW = Window.ARROW
M.IBEAM = Window.IBEAM
M.CONTEXTUAL_MENU = Window.CONTEXTUAL_MENU
M.CROSSHAIR = Window.CROSSHAIR
M.CLOSED_HAND = Window.CLOSED_HAND
M.DRAG_COPY = Window.DRAG_COPY
M.DRAG_LINK = Window.DRAG_LINK
M.NO_OP = Window.NO_OP
M.OPEN_HAND = Window.OPEN_HAND
M.POINTING_HAND = Window.POINTING_HAND
M.RESIZE_LEFT = Window.RESIZE_LEFT
M.RESIZE_RIGHT = Window.RESIZE_RIGHT
M.RESIZE_LEFTRIGHT = Window.RESIZE_LEFTRIGHT
M.RESIZE_UP = Window.RESIZE_UP
M.RESIZE_DOWN = Window.RESIZE_DOWN
M.RESIZE_UPDOWN = Window.RESIZE_UPDOWN
M.DISAPPEARING_ITEM = Window.DISAPPEARING_ITEM

--- Constructor
-- @param init Constructor properties
-- @param init.ctx The context name
-- @name Context
function C:__call(init)
	assert(init.ctx)
	local m = setmetatable(init, M)
	
	m.dim = {512, 512}
	
	-- layers
	m.layers = {
		popup = Layer{
			context = m
		},
		floating = Layer{
			context = m
		},
		widgets = Layer{
			context = m
		},
	}
	-- set layer order
	m.layers[1] = m.layers.popup
	m.layers[2] = m.layers.floating
	m.layers[3] = m.layers.widgets
	
	m.mouse_state = {
		x = 0, y = 0,
		dx = 0, dy = 0,
		btn = "left",
	}
	
	m.keyboard_state = {
		key = 32,
		state = "up",
		
		shift = false,
		alt = false,
		ctrl = false,
		cmd = false,
	}
	
	m.mouse_handler = m:create_mouse_handler()
	
	m.colors = {
		border_top = {1, 1, 1, 0.35},
		border_bot = {1, 1, 1, 0.55},
		background = {1, 1, 1, 0.2},
	}
	
	m.fonts = {}
	
	return m
end

--- Get a font for a particular size
-- @param size The size
function M:get_font(size)
	local label = Label{
		ctx = self.ctx,
		fontfile = font_path, 
		size = size,
		alignment = Label.LEFT,
		font = self.fonts[size],
	}

	self.fonts[size] = label.font

	return label
end

function M:set_cursor_style(style)
	if(self.win) then
		self.win.cursorstyle = style
	end
end

--- Get a view at a point
-- @param x x-coordinate
-- @param y y-coordinate
-- @ret The view's layer
-- @ret The view
function M:view_at_point(x, y)
	for i, l in ipairs(self.layers) do
		local v = l:view_at_point(x, y)
		if(v) then
			return l, v
		end
	end
end

--- Draw the context
function M:draw()
	g.disable(g.Depth)
	g.enable(g.Blend)
	g.blend(g.SourceAlpha, g.One)
	g.enter2d(self.dim)

	for i=#self.layers, 1, -1 do
		local l = self.layers[i]
		l:draw()
	end
	
	g.leave2d()
end

--- Resize the context
-- @param dim The dimensions
function M:resize(dim)
	self.dim = dim
end


function M:copy()
	if(
		self.focused_view and 
		self.focused_view.copy
	) then
		app.clipboard(self.focused_view:copy())
	end
end

function M:cut()
	if(
		self.focused_view and 
		self.focused_view.cut
	) then
		app.clipboard(self.focused_view:cut())
	end
end

function M:paste()
	if(
		self.focused_view and 
		self.focused_view.paste
	) then
		self.focused_view:paste(app.clipboard())
	end
end

function M:selectall()
	if(
		self.focused_view and 
		self.focused_view.selectall
	) then
		self.focused_view:selectall()
	end
end

function M:modifiers(view)
	self.keyboard_state.shift = view.shift
	self.keyboard_state.alt = view.alt
	self.keyboard_state.ctrl = view.ctrl
	self.keyboard_state.cmd = view.cmd
end

--- Create a key event
-- @param e The event name
-- @param k The key
function M:key(e, k)
	self.keyboard_state.state = e
	self.keyboard_state.key = k
	if(self.focused_view) then
		if(e == "down") then
			if(self.focused_view.key_down) then
				self.focused_view:key_down(k)
			end
		else
			if(self.focused_view.key_up) then
				self.focused_view:key_up(k)
			end
		end
	end
end

--- Create a mouse event
-- @param e The event name
-- @param btn The button name
-- @param x x-coordinate
-- @param y y-coordinate
-- @param nclk The number of clicks
function M:mouse(e, btn, x, y, nclk)
	self.mouse_state.dx = x - self.mouse_state.x
	self.mouse_state.dy = y - self.mouse_state.y
	self.mouse_state.x = x
	self.mouse_state.y = y
	
	self.mouse_handler(e, btn, x, y, nclk)
end

function M:create_mouse_handler()
	local prev_l, prev_v
	
	local f = co.wrap(function()
		while(true) do
			local e, btn, x, y, nclkk = co.yield()
			
			local l, v = self:view_at_point(x, y)
			-- mouse enter/exit (mouse movement)
			if(v) then
				if(v ~= prev_v) then
					if(prev_v and prev_v.mouse_exit) then
						prev_v:mouse_exit()
					end
				end

				if(v.mouse_enter) then
					v:mouse_enter()
				end
				
				prev_l, prev_v = l, v
			elseif(prev_v) then
				if(prev_v.mouse_exit) then
					prev_v:mouse_exit()
				end
			end
		
			-- mouse down/drag/up
			if(e == "down") then
				if(v ~= self.focused_view) then		
					if(self.focused_view and self.focused_view.loose_focus) then
						self.focused_view:loose_focus()
					end
					
					if(v and v.gain_focus) then
						v:gain_focus()
					end
					self.focused_view = v
				end
			
				if(btn == "left") then
					-- try to click on a view
					if(v) then
						if(v.mouse_down) then
							v:mouse_down(btn, x - v.rect.x, y - v.rect.y, nclks)
						end
						
						repeat
							e, btn, x, y, nclk = co.yield()
							if(e ~= "drag") then
								break
							end
							if(v.mouse_drag) then
								v:mouse_drag(btn, x - v.rect.x, y - v.rect.y)
							end
						until(e ~= "drag")
						
						if(v.mouse_up) then
							v:mouse_up(btn, x - v.rect.x, y - v.rect.y)
						end
					else
						self.focused_view = nil
					
						-- holding pattern
						repeat
							e, btn, x, y, nclk = co.yield()
						until(e ~= "drag")
						
					end
				elseif(btn == "right") then
					
				end
			end
		end
	end)
	f()
	return f
end

--- Add a view to the context
-- @arapm v The view
function M:add_view(v)
	v.context = self.context
	self.layers.widgets:add_view(v)
end