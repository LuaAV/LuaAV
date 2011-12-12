local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Array = require("Array")

local ctx = "Test OpenGL Textures"
win = Window(ctx, 200, -1000, 1200, 800)

local char1 = Array(1, Array.UInt8, {32, 32})
local char2 = Array(2, Array.UInt8, {32, 32})
local char3 = Array(3, Array.UInt8, {32, 32})
local char4 = Array(4, Array.UInt8, {32, 32})

local float1 = Array(1, Array.Float32, {32, 32})
local float2 = Array(2, Array.Float32, {32, 32})
local float3 = Array(3, Array.Float32, {32, 32})
local float4 = Array(4, Array.Float32, {32, 32})


for i=0, char1.dim[1]-1 do
for j=0, char1.dim[2]-1 do
	char1:setcell(i, j, {255*i/(char1.dim[1]-1)})
end
end

for i=0, char2.dim[1]-1 do
for j=0, char2.dim[2]-1 do
	char2:setcell(i, j, {255*i/(char2.dim[1]-1), 128})
end
end

for i=0, char3.dim[1]-1 do
for j=0, char3.dim[2]-1 do
	char3:setcell(i, j, {
		255*i/(char3.dim[1]-1), 
		255*j/(char3.dim[2]-1), 
		255*(1-j/(char3.dim[2]-1))
	})
end
end

for i=0, char4.dim[1]-1 do
for j=0, char4.dim[2]-1 do
	char4:setcell(i, j, {
		255*i/(char4.dim[1]-1), 
		255*j/(char4.dim[2]-1), 
		255*(1-j/(char4.dim[2]-1)),
		128
	})
end
end


for i=0, float1.dim[1]-1 do
for j=0, float1.dim[2]-1 do
	float1:setcell(i, j, {i/(float1.dim[1]-1)})
end
end

for i=0, float2.dim[1]-1 do
for j=0, float2.dim[2]-1 do
	float2:setcell(i, j, {i/(float2.dim[1]-1), 0.5})
end
end

for i=0, float3.dim[1]-1 do
for j=0, float3.dim[2]-1 do
	float3:setcell(i, j, {
		i/(float3.dim[1]-1), 
		j/(float3.dim[2]-1), 
		(1-j/(float3.dim[2]-1))
	})
end
end

for i=0, float4.dim[1]-1 do
for j=0, float4.dim[2]-1 do
	float4:setcell(i, j, {
		i/(float4.dim[1]-1), 
		j/(float4.dim[2]-1), 
		(1-j/(float4.dim[2]-1)),
		0.5
	})
end
end

local char1_tex = Texture(ctx)
char1_tex:fromarray(char1)

local char2_tex = Texture(ctx)
char2_tex:fromarray(char2)

local char3_tex = Texture(ctx)
char3_tex:fromarray(char3)

local char4_tex = Texture(ctx)
char4_tex:fromarray(char4)


local float1_tex = Texture(ctx)
float1_tex:fromarray(float1)

local float2_tex = Texture(ctx)
float2_tex:fromarray(float2)

local float3_tex = Texture(ctx)
float3_tex:fromarray(float3)

local float4_tex = Texture(ctx)
float4_tex:fromarray(float4)


local
function show(tex, pos, scale)
	gl.PushMatrix()
		gl.Translate(pos)
		gl.Scale(scale)
--		gl.Scale(0.004, 0.004, 0.004)
		tex:bind()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			gl.TexCoord(0, 0)	gl.Vertex(0, 0)
			gl.TexCoord(1, 0)	gl.Vertex(1, 0)
			gl.TexCoord(1, 1)	gl.Vertex(1, 1)
			gl.TexCoord(0, 1)	gl.Vertex(0, 1)	
		gl.End()
		tex:unbind()
	gl.PopMatrix()
end


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	
	sketch.enter_ortho()
		---[[
		show(char1_tex, {-1, 0, 0}, {1/2, 1, 1})
		show(char2_tex, {-0.5, 0, 0}, {1/2, 1, 1})
		show(char3_tex, {0, 0, 0}, {1/2, 1, 1})
		show(char4_tex, {0.5, 0, 0}, {1/2, 1, 1})
		--]]
		show(float1_tex, {-1, -1, 0}, {1/2, 1, 1})
		show(float2_tex, {-0.5, -1, 0}, {1/2, 1, 1})
		show(float3_tex, {0, -1, 0}, {1/2, 1, 1})
		show(float4_tex, {0.5, -1, 0}, {1/2, 1, 1})
	sketch.leave_ortho()
end
