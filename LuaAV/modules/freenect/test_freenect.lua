local bit = require "bit"
local gl = require "opengl"
local Texture = require "opengl.Texture"
local Shader = require "opengl.Shader"
local sketch = require "opengl.sketch"

local ctx = "special K"
local win = Window(ctx, 0, 0, 640, 480)
local tex = Texture(ctx)

local glsl = [=[

Shader{
	name = "freenect.depth",
	description = [[ Render depth data from Freenect ]],
	language = "GLSL",
	parameters = {
		Parameter {
			0,
			name = "tex0", type = "int",
			description = "Input texture",
		},
	},

--------------------------------------------------------------------------
--------------------------------------------------------------------------
--	Vertex Program
--------------------------------------------------------------------------
Program{
	type = "vertex",[[

varying vec2 texcoord0;
void main() {
	gl_Position = ftransform();
	texcoord0 = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}

]]},
--------------------------------------------------------------------------
--------------------------------------------------------------------------
--	Fragment Program
--------------------------------------------------------------------------
Program{
	type = "fragment",[[
uniform sampler2D tex0;	
varying vec2 texcoord0;
void main() {
	float raw = texture2D(tex0, texcoord0).a;
	float d = 0.;
	// ignore out-of-range points:
	if (raw < 2047.) {
		d = sqrt(1.-raw/2048.);
	}
	gl_FragColor = vec4(d, d, d, 1);
}

]],},
--------------------------------------------------------------------------
}

]=]

local shader = Shader{
	ctx = ctx,
	source = glsl,
}

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

dev:start_depth()
dev:start_video()

function win:draw()

	local arr = dev:video()
	tex:fromarray(arr)
	gl.Translate(-1, 0, 0)
	tex:bind()
	gl.Begin(gl.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
	gl.Translate(1, 0, 0)
	
	local arr = dev:depth()
	tex:fromarray(arr)
	gl.Translate(1, 0, 0)
	tex:bind()
	shader:param("tex0", 0)
	shader:bind()
	gl.Begin(gl.QUADS)
		sketch.quad()
	gl.End()
	shader:unbind()
	tex:unbind()
	gl.Translate(-1, 0, 0)
end

