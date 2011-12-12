
local M = require("zengarden")

for k, v in pairs(M) do 
	print("module.", k, v)
	_G[k] = v
end


local context = assert(PdContext{})
local graph = assert(context:open(script.path.."/", "test.pd"))

--context.out = audio.root.out
print(audio.root, audio.root.out)

local p = context:prototype()
p.x = 1
print("proto:", p)
for k, v in pairs(p) do print("proto", k, v) end


print(context, graph)

wait(0.5)
context:send("freq", 440)

wait(1)
context:stop()


print('ok')
context = nil
graph = nil
collectgarbage()
collectgarbage()