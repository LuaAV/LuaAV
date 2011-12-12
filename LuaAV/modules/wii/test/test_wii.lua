require "wii"

local wiimote = wii.create()

function poll()
	local jx, jy = wiimote:joystick()
	local b, s = wiimote:button()
	local pitch, roll = wiimote:tilt()
	print(jx, jy, b, s, pitch, roll)
end

for i = 1, 10 do
	poll()
	wait(0.1)
end

wiimote = nil
collectgarbage()

print('done')