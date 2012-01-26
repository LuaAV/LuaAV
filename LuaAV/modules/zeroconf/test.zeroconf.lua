local zeroconf = require "zeroconf"

print("host:", zeroconf.hostname())

local client = zeroconf.Client()
print(client)

-- add callbacks to be notified when a service is added, resolved or removed:
client:added(function(name) print("added service", name) end)
client:resolved(function(name, host, port, address) print("resolved service", name, host, port, address) end)
client:removed(function(name) print("removed service", name) end)


local service = zeroconf.Service("LuaAV:"..zeroconf.hostname())
print(service)

wait(2)

service = nil
collectgarbage()
collectgarbage()