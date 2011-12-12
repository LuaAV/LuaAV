local wait = wait
local event = event
local go = go
local now = now
local type = type

local print = print
local setmetatable = setmetatable
local getfenv = getfenv

module(...)

local
function optbool(v, def)
	if(type(v) == "nil") then
		return def
	else
		return v
	end
end


local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M


function C:__call(init)
	init = init or {}
	local m = setmetatable(init, M)
	
	if(m.scheduler) then
		m.wait = m.scheduler.wait
		m.event = m.scheduler.event
		m.go = m.scheduler.go
		m.now = m.scheduler.now
	else
		m.wait = wait
		m.event = event
		m.go = go
		m.now = now
	end
	
	m.events = {}
	
	return m
end

function M:delayed_event(t, e)
	self.go(t, function()
		self.event(e)
	end)
end

function M:oneshot_delayed_event(t, e)
	if(not self.events[e]) then
		self.events[e] = true
		self.go(t, function()
			self.event(e)
			self.events[e] = nil
		end)
	end
end

function M:dur_cond(dur)
	local limit = dur+self.now()
	local start = self.now()
	
	return
	function()
		return 
			self.now() < limit,
			(self.now()-start)/dur
	end
end

function M:action(e)
	local cond = false
	self.go(e, function()
		cond = true
	end)
	
	return
	function()
		return cond
	end
end

function M:toggle(e, v)
	local cond = optbool(v, false)
	self.go(function()
		while(true) do
			self.wait(e)
			cond = not cond
		end
	end)
	
	return
	function()
		return cond
	end
end

-- wrap this in a go()
function M:driver(e, f, ...)
	local tog = self:toggle(e)
	while(true) do
		self.wait(e)
		if(tog()) then
			local a = self:action(e)
			self.go(f, a, ...)
		end
	end
end