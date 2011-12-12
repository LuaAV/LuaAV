

local win = Window("X")
win2 = Window("Y")

go(function()
	while(true) do
		collectgarbage()
		wait(0.1)
	end
end)