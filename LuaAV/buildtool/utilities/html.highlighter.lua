require "doc.lexer"

function highlight_pre(src)
       -- convert quotes
       local src = src:gsub("&quot;", '"')
       -- strip <pre> tag
       src:gsub("<pre>", ''):gsub("</pre>", '')

       local lexed = lexer(src)
       local str = {}
       for i, v in ipairs(lexed) do
               table.insert(str, string.format("<span class=%s>%s</span>", unpack(v)))
       end
       return "<pre>"..table.concat(str).."</pre>"
end

-- example:
local src = [==[
<pre>function make_counter()
       local count = 0
       return function()
               count = count + 1
               print(count)
       end
end

-- call to make_counter() returns a function;
-- and 'captures' the local <em>count</em> as an 'upvalue' specific to it
local c1 = make_counter()
c1()    -- prints: 1
c1()    -- prints: 2
c1()    -- prints: 3


-- another call to make_counter() creates a new function,
-- with a new <em>count</em> upvalue
local c2 = make_counter()
c2()    -- prints: 1
c2()    -- prints: 2

-- the two function's upvalues are independent of each other:
c1()    -- prints: 4
</pre>]==]

local src = [==[
win = Window("Canvas", 0, 0, 512, 512)

-- define a resize event handler
function win:resize()
	local dim = self.dim
	print("new dim: ", dim[1], dim[2])
end

]==]

local src = [==[
Window("Canvas", 0, 0, 512, 512)
]==]

local src = [==[
local gl = require("opengl")
local GL = gl

function win:draw()
	gl.Begin(GL.POINTS)
		gl.Vertex(0, 0, 0)
	gl.End()
end
]==]

local src = [==[
local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local image = require("image")
local Image = image.Image

local ctx = "Load Image"
win = Window(ctx, 0, 0, 512, 512)

local image = Image(LuaAV.findfile("LuaAV.96.png"))
local tex = Texture(ctx)
tex:frommatrix(image:matrix())

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.Color(1, 1, 1, 1)
	gl.Scale(0.5, 0.5, 0.5)
	tex:bind()
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
end
]==]

local src = [==[
local gl = require("opengl")
local GL = gl

win = Window("test")

function drawstuff()
	while true do
		gl.Begin(gl.LINES)
			gl.Vertex(-math.cos(now()), math.sin(now()), 0)
			gl.Vertex(0, 0, 0)
		gl.End()
		wait("draw")
	end
end

go("draw", drawstuff)

function win:draw()
	-- resume any functions waiting on the draw event
	event("draw")
	
end
]==]

print("")
print(highlight_pre(src))