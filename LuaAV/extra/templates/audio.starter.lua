local util = require "audio.util"
local driver = require "audio.driver"
local Def = require "audio.Def"
-- places all Def generators in global namespace:
Def.globalize()		

-- uncomment this function to track audio CPU usage:
-- go(function() while true do print(driver.status()); wait(1); end end)



