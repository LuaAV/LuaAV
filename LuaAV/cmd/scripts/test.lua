
print "test.lua loaded"

for k, v in pairs(script) do print("script", k, v) end
for k, v in pairs(LuaAV) do print("LuaAV", k, v) end

function onclose()
	print("closing", script.name)
end

-- child states:
---[[
child = LuaAV.new("test2")
print("child:", child)
child:dofile("test2.lua", script.path)
child:dostring("print(...)", 1, 2, 3)
--]]

---[[
-- scheduler:
local coro = go(function()
	while now() < 2 do
		print(script.name, now())
		wait(1)
		
		if now() > 1 then
			child:close() -- = nil
			collectgarbage()
		end
	end
	
end)

--]]

