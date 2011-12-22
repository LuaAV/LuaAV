local Page = require("Page")
local utils = require("utilities")
local read_file = utils.read_file

local script = script

module(...)



tutorials = {
	Page{
		filename = "tutorial.01.introduction.to.LuaAV.html",
		title = "Introduction to LuaAV",
		content = {
			title = "Introduction to LuaAV",
			text = read_file(script.path.."/doc.tutorials/introduction.to.LuaAV.html"),
		},
	},
	Page{
		filename = "tutorial.02.introduction.to.Lua.html",
		title = "Introduction to Lua",
		content = {
			title = "Introduction to Lua",
			text = read_file(script.path.."/doc.tutorials/introduction.to.Lua.html"),
		},
	},
	Page{
		filename = "tutorial.03.timing.and.scheduling.html",
		title = "Timing and Scheduling",
		content = {
			title = "Timing and Scheduling",
			text = read_file(script.path.."/doc.tutorials/timing.and.scheduling.html"),
		},
	},
	Page{
		filename = "tutorial.04.audio.tutorial.1.html",
		title = "Making Sound",
		content = {
			title = "Making Sound",
			text = read_file(script.path.."/doc.tutorials/audio.tutorial.1.html"),
		},
	},
	Page{
		filename = "tutorial.05.graphics.tutorial.1.html",
		title = "Drawing Shapes",
		content = {
			title = "Drawing Shapes",
			text = read_file(script.path.."/doc.tutorials/graphics.tutorial.1.html"),
		},
	},
	Page{
		filename = "tutorial.06.multi.channel.audio.html",
		title = "Multi-Channel Audio",
		content = {
			title = "Multi-Channel Audio",
			text = read_file(script.path.."/doc.tutorials/multi.channel.audio.html"),
		},
	},
	Page{
		filename = "tutorial.07.multi.channel.audio.html",
		title = "Drawing with Images",
		content = {
			title = "Drawing with Images",
			text = read_file(script.path.."/doc.tutorials/graphics.tutorial.2.html"),
		},
	},
}

mainpages = {
	Page{
		filename = "index.html",
		title = "LuaAV Overview",
		tabtitle = "Main Page",
		content = {
			title = "LuaAV Overview",
			text = [[
<h2><a class="anchor" id="chapter_About"></a>About</h2>
<p>LuaAV is an integrated programming environment based upon extensions to the Lua programming language to enable the tight real-time integration of computation, time, sound and space. It is intended to support:
</p>

<ul>
<li>Computational Audiovisual Composition</li>
<li>Interactive arts</li>
<li>Multimedia development research</li>
<li>Scientific visualization</li>
<li>Research application development</li>
</ul>


<h2><a class="anchor" id="chapter_LuaAV_Concept"></a>LuaAV Concept</h2>
<p>
LuaAV is a real-time audiovisual scripting environment based around the Lua scripting language and a collection of libraries for sound, graphics, and media protocols. The goal of LuaAV is to serve as a computational platform for creative exploration that is both fluid and generative. As with the Lua language, the design strategies for LuaAV favor mechanisms over implementation in order to keep its structure as clear, compact, dynamic, and reconfigurable as possible. At any given moment, it should be possible to enact any creative thought about sound, image, space, and time and experiment with a range of possible approaches without having to divert energy away from its pursuit due to the brittleness of the computational system. Computation must become plastic. Software must become more soft.
</p>

<p>
LuaAV is a generative kernel of computation, capable of spawning new structures and procedures dynamically. In essence, it is meta-software for audiovisual composition capable of generating a vast range of possible softwares either at the hand of a user/programmer or even algorithmically through abstract machines. As much of LuaAV as is reasonable is generated dynamically through built-in compiler, parser, and code generating mechanisms such that the software grows and adapts as features are employed.
</p>

<p>
LuaAV is divided between a cross-platform library called libluaav and collection platform-specific classes and functions that make up the LuaAV application. libluaav contains the core timing, scheduling, and multi-threaded messaging functionality and is designed in such a way that it can be embedded in a wide variety of contexts much like Lua itself. Possible contexts might include VST plugins, web-browser plugins, streaming radio stations, or other host applications such as 3D modeling programs. In such contexts platform-specific functionality such as windowing and menus are irrelevant, which is why they are in the LuaAV application instead of libluaav.
</p>
]]
		},
	},
	Page{
		filename = "manual.html",
		title = "LuaAV Manual",
		tabtitle = "Manual",
		content = {
			title = "LuaAV Manual",
			text = [[
			
]]
		},
	},
	Page{
		filename = "reference.html",
		title = "LuaAV Reference",
		tabtitle = "Reference",
		content = {
			title = "LuaAV Reference",
			text = [[
<p>The reference manual covers the core LuaAV modules.  Module names that are capitalized represent modules that are object classes and can be used as constructors with the exception of the <b>LuaAV</b> module.  For example, the <b>opengl.Texture</b> module can be used to directly construct a <b>Texture</b> object:</p>

<luacode>
local Texture = require("opengl.Texture")
local tex = Texture("context")
</luacode>

<p>Objects in LuaAV such as <b>Texture</b> can have both <i>attributes</i> and <i>functions</i>.  Attributes are properties of an object and are accessed using the '.' notation.  Functions are actions an object can perform and are called using the ':' notation denoting instance method.</p>

<luacode>
-- setting an attribute
tex.dim = {1024, 1024}
-- getting an attribute
print(unpack(tex.clearcolor))

-- calling a function
tex:bind()
</luacode>
]]
		},
	},
}