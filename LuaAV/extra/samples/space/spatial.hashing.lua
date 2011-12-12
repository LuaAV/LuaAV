local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")
local Mesh = require("opengl.Mesh")

local sketch = require("opengl.sketch")
local HSpace = require("space.HSpace")
local Camera = require("glutils.navcam")

local color = require("color")
color.milk = {0.93, 0.93, 0.93}

local Label = require("Label")

local sqrt = math.sqrt
local abs = math.abs
local max = math.max
local min = math.min
local floor = math.floor

local ctx = "Hashing Test"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = color.milk

local cam = Camera{}

local label = Label{
	ctx = ctx,
	size = 14,
	alignment = Label.CENTER,
	valignment = Label.CENTER,
}

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
	cam:key(self, e, k)
end

function win:resize()
	cam:resize(self)
end


local SZ = 2^5
local hspace = HSpace(SZ)

local SizePTS = 400
local points = Texture{
	ctx = ctx,
	type = GL.FLOAT,
	format = GL.RGB,
	dim = {SizePTS, SizePTS},
}
print("number of points:", SizePTS*SizePTS)

local gen = Slab{
	ctx = ctx,
	file = LuaAV.findfile("gn.perlin.noise.points.shl"),
	param = {
		scale = {SZ, SZ, SZ},
		xscale = {10, 10},
		yscale = {10, 10},
		zscale = {10, 10},
		--offset = {0.5, 0.7},
	}
}
gen:texture().type = points.type
gen:texture().format = points.format
gen:texture().dim = points.dim

local mesh = Mesh{
	ctx = ctx,
	primitive = GL.POINTS
}


function cube(x, y, z)
	gl.Begin(GL.LINE_LOOP)
		gl.Vertex(x, y, z)
		gl.Vertex(x, y+1, z)
		gl.Vertex(x+1, y+1, z)
		gl.Vertex(x+1, y, z)
	gl.End()
	
	gl.Begin(GL.LINE_LOOP)
		gl.Vertex(x, y, z+1)
		gl.Vertex(x, y+1, z+1)
		gl.Vertex(x+1, y+1, z+1)
		gl.Vertex(x+1, y, z+1)
	gl.End()
	
	gl.Begin(GL.LINES)
		gl.Vertex(x, y, z)
		gl.Vertex(x, y, z+1)
		
		gl.Vertex(x, y+1, z)
		gl.Vertex(x, y+1, z+1)
		
		gl.Vertex(x+1, y+1, z)
		gl.Vertex(x+1, y+1, z+1)
		
		gl.Vertex(x+1, y, z)
		gl.Vertex(x+1, y, z+1)
	gl.End()
end

go("draw", function()
	gen:draw(points)
	gen:draw(points)
	--[[
	local tex = gen:texture()
	tex:toarray()
	
	mesh:vertex(tex:array())
	hspace:hash(tex:array())
	--]]
	
	while(now() < 130) do
		gen:param("xoffset", {now(), now()*0.1})
		gen:param("yoffset", {now()*0.5+100, -now()*0.1})
		gen:draw(points)
		local tex = gen:texture()
		tex:toarray()
		
		mesh:vertex(tex:array())
		hspace:hash(tex:array())
		wait(1/33)
	end
end)

local clang = require("clang")
local cc = clang.Compiler()
local clang_headers = app.resourcepath .. "/Headers"
cc:include(clang_headers)
cc:compile[[

#include "lua_utility.h"
#include "lua_glue.h"
#include "HSpace.h"
#include <OpenGL/gl.h>

void cube(float x, float y, float z) {
	glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, z);
		glVertex3f(x, y+1, z);
		glVertex3f(x+1, y+1, z);
		glVertex3f(x+1, y, z);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, z+1);
		glVertex3f(x, y+1, z+1);
		glVertex3f(x+1, y+1, z+1);
		glVertex3f(x+1, y, z+1);
	glEnd();
	
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y, z+1);
		
		glVertex3f(x, y+1, z);
		glVertex3f(x, y+1, z+1);
		
		glVertex3f(x+1, y+1, z);
		glVertex3f(x+1, y+1, z+1);
		
		glVertex3f(x+1, y, z);
		glVertex3f(x+1, y, z+1);
	glEnd();
}


extern "C" int hspace_debug(lua_State *L) {
	if(lua_isuserdata(L, 1)) {
		HSpace *s = Glue<HSpace>::checkto(L, 1);
		int size = s->mSize;
/*
		glBegin(GL_POINTS);
		for(int i=0; i < s->mNObjects; i++) {
			HObject &o = s->mObjects[i];
			if(o.cellidx == InvalidCell) {
				glColor4f(1, 0.2, 0.2, 1);
			}
			else {
				glColor4f(0., 0., 0., 1);
			}
			glVertex3f(o.x, o.y, o.z);
		}
		glEnd();
		*/

		glColor4f(1., 0., 0., 0.045);
		for(int i=0; i < size*size*size; i++) {
			HCell &cell = s->mCells[i];
			if(cell.objects) {
				int x, y, z;
				s->cellCoords(i, x, y, z);
				cube((float)x, (float)y, (float)z);
			}
		}
	}
	return 0;
}

]]
local jit = assert(cc:jit(), "unable to JIT-compile")
local hspace_debug = assert(jit:pushcfunction("hspace_debug"), "JITted define-function not found")



function win:draw()
	gl.Enable(GL.BLEND)
	gl.Disable(GL.DEPTH_TEST)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.PointSize(1)
	
	cam:step()
	cam:enter()
	
	event("draw")
	
	--gl.Rotate(90, -1, 0, 0)
	gl.Scale(1/SZ, 1/SZ, 1/SZ)
	
	gl.PushMatrix()
		gl.Color(1, 0.2, 0.2, 0.3)
		gl.Scale(SZ, SZ, SZ)
		cube(0, 0, 0)
	gl.PopMatrix()
		
	--[[
	local tex = gen:texture()
	tex:bind()
	gl.Begin(GL.QUADS)
		gl.Color(1, 1, 1, 1)
		sketch.quad()
	gl.End()
	tex:unbind()
	--]]
	--gl.Color(1, 0, 0, 0.2)
	gl.Color(0, 0, 0, 0.2)
	mesh:draw()
	
	gl.Color(0, 0, 0, 1)
	hspace_debug(hspace)
	
	cam:leave()
end