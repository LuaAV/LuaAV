local zeroconf = require "zeroconf"

print(zeroconf.hostname())

local client = zeroconf.Client()
print(client)

local service = zeroconf.Service(zeroconf.hostname() .. "_myfunkyservice")
print(service)

while true do
	--client:poll()
	wait(1)
end
