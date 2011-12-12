require "lfs"
require "app"

package.path = app.resourcepath .. app.separator .. "?.lua;" .. package.path

require "audio.driver"
require "app.console"
require "app.screens"	-- needed, especially on Linux to init some statics
local rocksmanager = require "app.rocksmanager"


local utilities = require "app.utilities"
utilities.ensurepath(utilities.stripfilename(app.logpath))
local f = assert(io.open(app.logpath, "w"))
f:close()

app.console.startlogging(app.logpath)


local App = require "app.app"
local application = App()

---------------------------------------
-- test:
---------------------------------------

function onclose()
	-- write out app data to config file ...
	application:quit()
end

function fileopen(filename)
	application:fileopen(filename)
end

function fileclose(filename)
	application:fileclose(filename)
end

function fileedit(filename)
	application:fileedit(filename)
end

function menuitem_exists(menuitem_path)
	return application:menuitem_exists(menuitem_path)
end

function addmenuitem(menupath, name, f)
	application:addmenuitem(menupath, name, f)
end

function menuitem_action(menu_path)
	return application:get_menuitem(menu_path).action
end

function rocksmanager_search(reponame)
	return rocksmanager.search(reponame)
end

function rocksmanager_install(reponame, name, version)
	return rocksmanager.install(reponame, name, version)
end


--for k, v in pairs(audio.driver) do print("audio.driver", k, v) end
--for k, v in pairs(audio.driver.inputs) do print("audio.driver.inputs", k, v) end
--for k, v in pairs(audio.driver.outputs) do print("audio.driver.outputs", k, v) end
--for k, v in pairs(Menu) do print("app.Menu", k, v) end
