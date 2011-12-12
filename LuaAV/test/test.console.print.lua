local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")

local ctx = "XX"
win = Window(ctx, 0, -800, 800, 600)
win.fps = 60

local tex = Texture(ctx)
tex.dim = {1024, 1024}

local cpu = LuaAV.time.cpu

local times = {}

go(function()
	local ct1, ct2 = cpu(), cpu()
	local t1, t2 = now(), now()
	while(true) do
		wait("draw")
		
		ct1, ct2 = ct2, cpu()
		t1, t2 = t2, now()
		local dct = ct2-ct1
		local dt = t2-t1
		--[[
		print("cpu: ", dct)
		print("now: ", dt)
		print(dct/dt)
		print("")
		--]]
		times[#times+1] = dct/dt
		if(#times > 100) then
			table.remove(times, 1)
		end
		if(dct/dt > 9) then
--			break
		end
	end
end)

function win:key(e, k)
	if(e == "down" and k == 27) then
		self.fullscreen = not self.fullscreen
	end
end

function win:draw()
	event("draw")
	tex:startcapture()
	tex:endcapture()
	
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	gl.Color(1, 1, 1, 0.3)
	gl.Begin(GL.LINES)
	for i=0, 10 do
		gl.Vertex(-2, i/3-0.5, 0)
		gl.Vertex(2, i/3-0.5, 0)
	end
	gl.End()
	
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.LINE_STRIP)
	for i, t in ipairs(times) do
		gl.Vertex(i/100*2-1, t/3-0.5, 0)
	end
	gl.End()
end