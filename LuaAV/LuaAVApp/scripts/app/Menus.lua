local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local collectgarbage = collectgarbage
local package = package

local print = print
local setmetatable = setmetatable

local string = require 'string'
local table = require 'table'
local io = require 'io'
local lfs = require 'lfs'

local openpanel = LuaAV.openpanel
local fileopen = LuaAV.fileopen
local fileload = fileopen
local MenuBar = MenuBar
local Menu = Menu
local MenuItem = MenuItem
local quit = LuaAV.quit
local fileclose = LuaAV.fileclose

module (...)
----------------------------------------------------------------------------
-- app.Menus
----------------------------------------------------------------------------

function menufield(class, label)
	return
	function(t)
		local menu = class(t)
		menu.label = label
		return menu
	end
end

function findmenuitem(menu, name)
	local names = {}

	local lastidx = 0
	for n, idx in string.gmatch(name, ("(%(.+%))()%.")) do
		lastidx = idx
		names[#names+1] = n:sub(2, n:len()-1)
	end
	if(name:len() > lastidx) then
		local n = name:sub(lastidx+1)
		names[#names+1] = n:sub(2, n:len()-1)
	end

	local m = menu
	for i, n in ipairs(names) do
		if(m.menu) then
			m = m:menu(n)
		else
			if(m.item) then
				m = m:item(n)
			else
				break
			end
		end
		
		if(not m) then
			break
		end
	end
	
	return m
end

function addmenuitem(mainmenu, menupath, name, action)
	local menu = findmenuitem(mainmenu, menupath)

	local item = MenuItem{
		label = name,
		action = action,
		key = "w",
		keymask = Menu.Cmd,
	}

	menu:append(item)
end