local utils = require("app.utilities")
local filename = "/Applications/LuaAV/Workshop Folder/file.lua"

print( utils.strippath(filename) )
print( utils.stripfilename(filename) )