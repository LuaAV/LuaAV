print(_VERSION)
local bit = require "bit"

local freenect = require "freenect"
for k, v in pairs(freenect) do print("freenect", k, v) end

print(freenect.num_devices(), "devices")
freenect.select_subdevices(bit.bor(freenect.device_flags.DEVICE_CAMERA, freenect.device_flags.DEVICE_MOTOR))

local dev = freenect.open_device(0)
print(dev)

local mode = dev:get_current_video_mode()
for k, v in pairs(mode) do print("video mode", k, v) end
local mode = dev:get_current_depth_mode()
for k, v in pairs(mode) do print("depth mode", k, v) end


dev:set_tilt_degs(-10)

dev:update_tilt_state()
local tilt = dev:get_tilt_state()
for k, v in pairs(tilt) do print("tilt", k, v) end

--[[
dev:start_depth()
dev:start_video()
print("started")

for i = 1, 1000 do
	freenect.process_events()
	--dev:update_tilt_state()
	wait(0.1)
end

dev:stop_depth()
dev:stop_video()
print("stopped")
--]]

--local ffi = require "ffi"
--print(ffi)



