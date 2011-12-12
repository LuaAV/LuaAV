print(script.name, now())

function onclose()
	print("closing", script.name)
end

local coro = go(function()
	while now() < 1 do
		print(script.name, now())
		wait(.1)
	end
end)

print(coro)