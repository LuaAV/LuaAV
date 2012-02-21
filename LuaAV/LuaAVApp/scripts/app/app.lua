module(..., package.seeall)

-- dependecies
local math = require "math"
local string = require "string"
local table = require "table"
local io = require "io"
local os = require "os"
local lfs = require "lfs"
local LuaAV = LuaAV
local app = require "app"
local Menu = require "app.Menu"
local MenuBar = require "app.MenuBar"
local MenuItem = require "app.MenuItem"
local configuration = require "app.configuration"
local Menus = require "app.Menus"
--local CodePad = require "app.CodePad"

local findfileinpath = app.utilities.findfileinpath
local strippath = app.utilities.strippath
local stripfilename = app.utilities.stripfilename
local getminimalpath = app.utilities.getminimalpath

local pairs = pairs
local ipairs = ipairs
local type = type
local print = print
local pcall = pcall
local tostring = tostring

-- setup basic OOP
local M = getfenv()
local C = {}
setmetatable(M, C)

-- Constructor
function C:__call(init)
	local m = {}
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.recent_docs_limit = 10
	m.openrecent_items = {}
	m.close_items = {}
	m.edit_items = {}
	m.scripts = {}
	m.codepads = {}
	m.untitled_count = 0
	
	m.config = configuration.load_configfile()
	m.last_dir = m.config.last_opendialog_dir
	m.mainmenu = m:generate_menus()
	
	if(m.config.console_rect) then
		app.console.rect(m.config.console_rect)
	end
	
	local startup = app.apppath.."/startup.lua"
	local ok, res = pcall(io.open, startup)
	if(res) then
		res:close()
		m:fileopen(startup)
	end
	
	return m
end

function M:new_codepad()
	-- generate a temp filename
	local filename = "Untitled"
	if(self.untitled_count > 0) then
		filename = filename .. " "..self.untitled_count
	end
	self.untitled_count = self.untitled_count+1

	-- setup child state
	local child = LuaAV.new(filename)

	-- load in preferences
	local ok, res = pcall(child.dofile, child, "preferences.lua", app.resourcepath)
	if(not ok) then
		print("Loading Preferences:", res)
	end
	
	-- create a CodePad
	local ok, res = pcall(child.gostring, child, [[
		local name = ...
		local CodePad = require("codepad.codepad")
		codepad = CodePad(name)
	]], filename)
	if(not ok) then
		print("creating CodePad", res)
	end

	-- update console
	app.console.addscript(filename, child)

	self.codepads[filename] = child
	self.codepads[child] = true	-- boolean signals a script never saved to file
end

function M:open_codepad(filename)
	if(self.codepads[filename]) then
		self:fileclose(filename)
	end
	
	print(string.rep("-", 40))
	print("opening CodePad: "..filename)	
	print(string.rep("-", 40))
	
	-- setup child state
	local name = strippath(filename)
	local path = stripfilename(filename)
	local child = LuaAV.new(filename)

	-- load in preferences
	local ok, res = pcall(child.dofile, child, "preferences.lua", app.resourcepath)
	if(not ok) then
		print("Loading Preferences:", res)
	end
	
	-- add file path to search paths
	local ok, res = pcall(child.gostring, child, [[
		local path = ...
		--print("adding path", path)
		LuaAV.addmodulepath(path)
		LuaAV.addsearchpath(path)
	]], path)
	if(not ok) then
		print("Adding search path", res)
	end
	
	-- load file
	local ok, res = pcall(child.dofile, child, name, path)
	if(not ok) then
		print(res)
	end	
	
	-- create a CodePad with script code
	local ok, res = pcall(child.gostring, child, [[
		local name = ...
		local CodePad = require("codepad.codepad")
		codepad = CodePad(name)
		
		local f = io.open(name, "r")
		local code = f:read("*a")
		codepad:text(code)
	]], filename)
	if(not ok) then
		print("creating CodePad", res)
	end

	-- update console and menus
	app.console.addscript(filename, child)
	self:add_open_recent(self.mainmenu, filename)
	self:add_close_file(self.mainmenu, filename)

	self.codepads[filename] = child
	self.codepads[child] = filename
end

function M:save_codepad(name)
	-- save the CodePad
	local child = self.codepads[name]
	if(child) then
		-- check to see if it has previously been save to file
		local filename = self.codepads[child]
		if(type(filename) ~= "string") then
			-- if not, open a dialog to save it
			filename = app.savepanel()
			if(not filename) then
				return
			else
				-- if we're saving, propagate the new name
				self.codepads[name] = nil
				
				self.codepads[filename] = child
				self.codepads[child] = filename

				-- change name of tab
				pcall(child.gostring, child, [[
					local filename = ...
					codepad:name(filename)
				]], filename)

				-- update console
				app.console.removescript(name)
				app.console.addscript(filename, child)
			end
		end

		-- grab the CodePad's text and write it to file
		pcall(child.gostring, child, [[
			local filename = ...
			local code = codepad:text()
			local f = io.open(filename, "w")
			f:write(code)
			f:close()
			codepad:name(filename)
		]], filename)
	end
end

function M:filenew(filename, code)
	if(filename and filename:len() > 0) then
		local code = code or "print('ok')"
		local f = io.open(filename, "w")
		f:write(code)
		f:close()
		self:fileopen(filename)
		self:fileedit(filename)
	end
end

function M:codepad_new()
	-- create new state
	-- execute code on state to create codepad
	
	local code = [[
		local CodePad = require("app.CodePad")
		codepad = CodePad()
	]]
end

function M:get_menuitem(menu_path)
	return Menus.findmenuitem(self.mainmenu, menu_path)
end

function M:fileopen(filename)
	if(self.codepads[filename]) then
		return self:open_codepad(filename)
	end
	
	local child
	local alreadyloaded = false
	local live_reload = false
	if(self.scripts[filename]) then
		child = self.scripts[filename]
		
		local _
		_, live_reload = pcall(child.dostring, child, [[
			if(script) then
				return script.live
			end
		]])
	
		if(not live_reload) then
			self:fileclose(filename, true)
			child = nil
		end
		
		alreadyloaded = true
	end
	
	print(string.rep("-", 40))
	print("opening: "..filename)	
	print(string.rep("-", 40))

	-- new child state
	local name = strippath(filename)
	local path = stripfilename(filename)
	child = child or LuaAV.new(name)

	if(not live_reload) then
		-- load in preferences
		local ok, res = pcall(child.dofile, child, "preferences.lua", app.resourcepath)
		if(not ok) then
			LuaAV.error("Loading Preferences:", res)
		end
		
		-- add file path to search paths
		local ok, res = pcall(child.gostring, child, [[
			local path = ...
			--print("adding path", path)
			LuaAV.addmodulepath(path)
			LuaAV.addsearchpath(path, true)
		]], path)
		if(not ok) then
			print("Adding search path", res)
		end
	end
	
	-- load file
	local ok, res = pcall(child.dofile, child, name, path)
	if(not ok) then
		LuaAV.error(res)
	end
	
	self.scripts[filename] = child
	self.scripts[child] = true
	
	-- update configuration and menus
	configuration.add_recentfile(self.config, filename)
	
	if(not alreadyloaded) then
		self:add_open_recent(self.mainmenu, filename)
		self:add_close_file(self.mainmenu, filename)
		self:add_edit_file(self.mainmenu, filename)
		
		-- update console
		if(not live_reload) then
			app.console.addscript(filename, child)
		end
	end
	
	-- watch file
	if(live_reload) then
		if app.file.watching(filename) then
			app.file.unwatch(filename)
		end
	end
	
	if not app.file.watching(filename) then
		app.file.watch(filename)
	end
end

function M:fileclose(filename, willreopen)
	local child = self.scripts[filename]
	if(child) then
		self.scripts[filename] = nil
		self.scripts[child] = nil
		child:close()
		
		if(not willreopen) then
			-- update menus
			self:remove_close_file(self.mainmenu, filename)
			self:remove_edit_file(self.mainmenu, filename)
			
			-- update console
			app.console.removescript(filename)
		end
		
		-- unwatch file
		if app.file.watching(filename) then
			app.file.unwatch(filename)
		end
	else
		child = self.codepads[filename]
		if(child) then
			self.codepads[filename] = nil
			self.codepads[child] = nil
			child:close()

			-- update console
			app.console.removescript(filename)
		end
	end
end

function M:fileedit(filename)
	-- only call for scripts, not codepads
	if(self.scripts[filename]) then
		if(LuaAV.platform == "LINUX") then
			app.fork("gedit " .. filename)
		else
			os.execute("open " .. filename:gsub("(%s)", "\\%0"))
		end
	end
end

function M:reload_script()
	local filename = app.console.selectedscript()
	if(filename) then
		if(self.scripts[filename]) then
			self:fileopen(filename)
		else
			-- codepad
		end
	end
end

-- 
function M:quit()
	print('quit')
	self.config.last_opendialog_dir = self.last_dir
	self.config.console_rect = app.console.rect()
	configuration.save_configfile(self.config)
end

function M:menuitem_exists(menuitem_path)
	return Menus.findmenuitem(self.mainmenu, menuitem_path)
end

function M:addmenuitem(menupath, name, f)
	local menu = Menus.findmenuitem(self.mainmenu, menupath)
	local item = MenuItem{
		label = name,
		action = f,
	}
	menu:append(item)
end

-- Generate App Menus
function M:generate_menus()
	local menufield = app.Menus.menufield
	local openpanel = app.openpanel
	local fileopen = app.fileopen
	local quit = app.quit
	
	-- generator separators
	local Separator = function()
		return MenuItem{
			separator = true,
		}
	end
	
	-- Menus and MenuItems
	local File = menufield(Menu, "File")
		local New = menufield(MenuItem, "New...")
		local NewFromTemplate = menufield(Menu, "New From Template")
		local Open = menufield(MenuItem, "Open...")
		local Edit = menufield(Menu, "Edit")
		local OpenRecent = menufield(Menu, "Open Recent")
		local Close = menufield(Menu, "Close")
		local Quit = menufield(MenuItem, "Quit")

	local Edit = menufield(Menu, "Edit")
		local Undo = menufield(MenuItem, "Undo")
		local Redo = menufield(MenuItem, "Redo")
		local Cut = menufield(MenuItem, "Cut")
		local Copy = menufield(MenuItem, "Copy")
		local Paste = menufield(MenuItem, "Paste")
		local SelectAll = menufield(MenuItem, "Select All")
	
	local Script = menufield(Menu, "Script")
		local Reload = menufield(MenuItem, "Reload")
	
	local MCodePad = menufield(Menu, "CodePad")
		--local CPNew = menufield(MenuItem, "New...")
		--local CPOpen = menufield(MenuItem, "Open...")
		local Eval = menufield(MenuItem, "Eval")
		--local Save = menufield(MenuItem, "Save")
	
	local Help = menufield(Menu, "Help")
		--local Docs = menufield(MenuItem, "LuaAV Help")
		local Site = menufield(MenuItem, "LuaAV Site")
		local Reference = menufield(MenuItem, "LuaAV Reference")

		
	-- Menu structure and generation
	local mainmenu = MenuBar{
		File{
			---[[
			New{
				action = function()
					local f = app.savepanel(last_dir, "New")
					if(type(f) == "string") then
						self.last_dir = stripfilename(f)
						self:filenew(f)
					end
				end,	
				key = "n",
				keymask = Menu.Cmd,
			},
			--]]
			
			-- Templates submenu
			NewFromTemplate{
			},
			
			---[[
			Open{
				-- closure to maintain state between calls
				action = function()
					local f = openpanel(last_dir)
					if(f) then
						if(type(f) == "string") then
							self.last_dir = stripfilename(f)
							self:fileopen(f)
						elseif(type(f) == "table") then
							self.last_dir = stripfilename(f[1])
							for i, f in ipairs(f) do
								self:fileopen(f)
							end
						end
					
						self.last_dir = last_dir
					end
				end,
				
				key = "o",
				keymask = Menu.Cmd,
			},
			
			OpenRecent{
			},
			
	--		Separator(),
			
			Edit{
			},
			
			Close{
			},
			
	--		Separator(),

			Quit{
				action = function()
					app.quit()
				end,

				key = "q",
				keymask = Menu.Cmd,
			},
			--]]
		},
		
		---[==[
		---[[
		Edit{
			Undo{
				action = function()
					app.window_event("undoText")
					--print("undo")
				end,
				
				key = "z",
				keymask = Menu.Cmd,
			},
			
			Redo{
				action = function()
					app.window_event("redoText")
					--print("app.redo")
				end,
				
				key = "Z",
				keymask = Menu.Cmd,
			},
			
			Separator(),
			
			Cut{
				action = function()
					app.window_event("cutText")
					--print("app.cut")
				end,
				
				key = "x",
				keymask = Menu.Cmd,
			},
			
			Copy{
				action = function()
					if(app.console.isfocused()) then
						app.console.copy()
					else
						app.window_event("copyText")
						--print("app.copy")
					end
				end,
				
				key = "c",
				keymask = Menu.Cmd,
			},
			
			Paste{
				action = function()
					app.window_event("pasteText")
					--print("app.paste")
				end,
				
				key = "v",
				keymask = Menu.Cmd,
			},
			
			SelectAll{
				action = function()
					if(app.console.isfocused()) then
						app.console.selectall()
					else
						app.window_event("selectAllText")
						--print("app.selectall")
					end
				end,
				
				key = "a",
				keymask = Menu.Cmd,
			},
		},
		--]]
		
		Script{
			Reload{
				action = function()
					self:reload_script()
				end,
				
				key = "r",
				keymask = Menu.Cmd,
			},
		},
		
		--[=[
		MCodePad{
		--[[
			CPNew{
				action = function()
					self:new_codepad()
				end,
				
				key = "n",
				keymask = Menu.Cmd + Menu.Alt,
			},
			
			CPOpen{
				action = function()
					local f = openpanel(last_dir)
					if(f) then
						if(type(f) == "string") then
							self.last_dir = stripfilename(f)
							self:open_codepad(f)
						elseif(type(f) == "table") then
							self.last_dir = stripfilename(f[1])
							for i, f in ipairs(f) do
								self:open_codepad(f)
							end
						end
					
						self.last_dir = last_dir
					end
				end,
			},

			Save{
				action = function()
					local name = CodePad.current_name()
					if(name) then
						self:save_codepad(name)
					end
				end,
				
				key = "s",
				keymask = Menu.Cmd,
			},
			
			Separator(),
		--]]
			Eval{
				action = function()
					app.window_event("eval")
				end,
				
				key = "enter",
				keymask = Menu.Cmd,
			},
		},
		--]=]
		
		Help{
			--[[	
			Docs{
				action = function()
					print("go to documentation")
				end,
			},
			--]]
			
			Site{
				action = function()
					app.openurl("http://lua-av.mat.ucsb.edu/");
				end,
			},
			Reference{
				action = function()
					app.openurl("file://"..app.apppath.."/extra/docs/index.html");
				end,
			}
		}
		--]==]
	}

	---[[
	local TemplateMenu = Menus.findmenuitem(mainmenu, "(File).(New From Template)")
	local template_folder = findfileinpath(app.apppath .. "/extra", "templates")
	if(not template_folder) then
		template_folder = findfileinpath(app.apppath .. "/../../extra", "templates")
	end
	
	if template_folder then
		local idx = 0
		local originalpath = lfs.currentdir()
		for f in lfs.dir(template_folder) do
			local fname = template_folder .. "/" ..f
			
			local attr = lfs.attributes(fname)
			if(attr.mode ~= "directory" and f:sub(1, 1) ~= ".") then
				TemplateMenu:insert(MenuItem{
					label = f,
					action = function()
						local f = app.savepanel(last_dir)
						if(type(f) == "string") then
							local code = io.open(fname):read("*a")
							self:filenew(f, code)
						end
					end,
				}, idx)
				idx = idx + 1
			end
		end
		lfs.chdir(originalpath)
	end
	--]]
	--[[
		MenuItem{
			label = mp,
			action = function()
				fileopen(script.name)
			end,
		}
	--]]
	
	self:make_openrecent_menu(mainmenu)

	return mainmenu
end

function M:make_openrecent_menu(mainmenu)
	local docs = self.config.recent_files
	
	local len = math.min(#docs, self.recent_docs_limit)
	for i=len, 1, -1 do
		self:add_open_recent(mainmenu, docs[i].name)
	end
end

function M:add_close_file(mainmenu, filename)
	local Close = Menus.findmenuitem(mainmenu, "(File).(Close)")
	local item = MenuItem{
		label = getminimalpath(self.openrecent_items, filename),
		action = function()
			self:fileclose(filename)
		end,
	}	
	Close:append(item)
	self.close_items[filename] = item
end

function M:remove_close_file(mainmenu, filename)
	local Close = Menus.findmenuitem(mainmenu, "(File).(Close)")
	local item = self.close_items[filename]
	Close:remove(item)
end

function M:add_edit_file(mainmenu, filename)
	local Edit = Menus.findmenuitem(mainmenu, "(File).(Edit)")
	local item = MenuItem{
		label = getminimalpath(self.openrecent_items, filename),
		action = function()
			self:fileedit(filename)
		end,
	}	
	Edit:append(item)
	self.edit_items[filename] = item
end

function M:remove_edit_file(mainmenu, filename)
	local Edit = Menus.findmenuitem(mainmenu, "(File).(Edit)")
	local item = self.edit_items[filename]
	Edit:remove(item)
end

function M:add_open_recent(mainmenu, filename)
	local OpenRecent = Menus.findmenuitem(mainmenu, "(File).(Open Recent)")	
	local item = self.openrecent_items[filename]
	if(item) then
		self.openrecent_items[filename] = nil
		
		for i, n in ipairs(self.openrecent_items) do
			if(n == filename) then
				table.remove(self.openrecent_items, i)
				break
			end
		end
	else
		-- Generate a new item
		item = MenuItem{
			label = getminimalpath(self.openrecent_items, filename),
			action = function()
				self:fileopen(filename)
			end,
		}
	end
	
	-- Insert into Menu
	OpenRecent:insert(item, 0)
	
	-- Save reference to MenuItem
	self.openrecent_items[filename] = item
	table.insert(self.openrecent_items, 1, filename)
	
	-- Shrink number of Open Recet MenuItems to limit
	while(#self.openrecent_items > self.recent_docs_limit) do
		local nitems = #self.openrecent_items
		local last_scriptname = self.openrecent_items[nitems]
		local item = self.openrecent_items[last_scriptname]

		self.openrecent_items[last_scriptname] = nil
		self.openrecent_items[nitems] = nil

		OpenRecent:remove(item)
	end
end
