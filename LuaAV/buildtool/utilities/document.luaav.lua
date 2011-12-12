local docroot = script.path.."/doc"

local doc = require("doc")
local doc_html = require("doc.doclet.html")
local doc_util = require("doc.util")

local
function repo_relative(subpath)
	return script.path.."/../.."..subpath
end

local mods = {
	--[[
	doc.module{
		name = "cgl",
		path = repo_relative("/extra/modules"),
		
		file = "cgl/init.lua",
		files = {
			"cgl/util.lua",
		},
	},

	
	doc.module{
		name = "doc",
		path = repo_relative("/extra/modules"),
		file = "doc/init.lua",
		
		files = {
			"doc/util.lua",
			"doc/path.lua",
			"doc/lang.lua",
			"doc/lang/lua.lua",
			"doc/doclet.lua",
			"doc/doclet/html.lua",
		},
	},
	

	doc.module{
		name = "language",
		path = repo_relative("/extra/modules"),
		file = "language/init.lua",
		
		mods = {
			doc.module{
				name = "language.specification",
				summary = "Specifications for different languages",
			},
		},
		
		files = {
			"language/specification/lua.lua",
		},
	},

	doc.module{
		name = "codegen",
		path = repo_relative("/extra/modules"),
		summary = "Code generation functionality including lexers, parsers, transformers, and serializers",
		
		files = {
		--	"codegen/compiler.lua",
			"codegen/lexer.lua",
			"codegen/parser.lua",
			"codegen/transformer.lua",
		},
	},
	--]]
	
	doc.module{
		name = "LuaAV",
		path = repo_relative("/extra/modules"),
		summary = "Core lib LuaAV",
		
		--file = "LuaAV/init.lua",
		files = {
			"LuaAV/time.lua",
			--"audio/util.lua",
			--"audio/driver.lua",
		},
	},
	
	doc.module{
		name = "midi",
		path = repo_relative("/libluaav/src"),
		summary = "Real-time MIDI IO",
		
		file = "luaav_midi.cpp",
	},
	
	doc.module{
		name = "osc",
		path = repo_relative("/libluaav/src"),
		summary = "OpenSoundControl IO",
		
		file = "luaav_osc.cpp",
	},
	
	doc.module{
		name = "audio",
		path = repo_relative("/extra/modules"),
		summary = "Audio synthesis",
		
		file = "audio/init.lua",
		files = {
			"audio/Def.lua",
			"audio/util.lua",
			"audio/driver.lua",
		},
	},
	
	--[[
	doc.module{
		name = "audio.soundfile",
		path = repo_relative("/libluaav/src"),
		
		file = "luaav_audio_soundfile.cpp",
	},
	--]]
	doc.module{
		name = "gui",
		path = repo_relative("/extra/modules"),
		summary = "Gui interface and widgets",
		
		files = {
			"gui/Button.lua",
			"gui/Context.lua",
			"gui/draw.lua",
			"gui/Label.lua",
			"gui/Layer.lua",
			"gui/Rect.lua",
			"gui/Slider.lua",
			"gui/View.lua",
		},
	},
	
	--[[
	doc.module{
		name = "serializer",
		path = repo_relative("/extra/modules"),
		
		file = "serializer/init.lua",
	},
	--]]
	
	doc.module{
		name = "Label",
		path = repo_relative("/extra/modules"),
		
		file = "Label/init.lua",
	},
	
	doc.module{
		name = "app",
		path = repo_relative("/LuaAVApp"),
		summary = "Modules for building applications from basic elements such as windows, menus, etc.",
		
		file = "src/luaav_app.cpp",
		files = {
			"src/luaav_app_window.cpp",
			"src/luaav_app_screens.h",
		--	"src/luaav_app_menu.cpp",
		--	"src/luaav_filewatcher.cpp",
		},
	},
	
	doc.module{
		name = "font",
		path = repo_relative("/modules/font"),
		
		file = "src/lua_font.cpp",
		files = {
			"src/lua_font_udata.cpp",
		}
	},
	
	doc.module{
		name = "opencl",
		path = repo_relative("/modules/opencl"),
		
		file = "src/lua_opencl.cpp",
	},
	---[[
	doc.module{
		name = "opengl",
		path = repo_relative("/modules/opengl"),
		
		file = "src/lua_opengl.cpp",
		files = {
			"src/lua_opengl_camera.cpp",
			"src/lua_opengl_mesh.cpp",
			"src/lua_opengl_texture.cpp",
			"src/lua_opengl_shader.cpp",
			"src/lua_opengl_slab.cpp",
			"src/lua_opengl_sketch.cpp",
		}
	},--]]
	
	doc.module{
		name = "Array",
		path = repo_relative("/libluaav/src"),
		
		file = "lua_array.cpp",
	},
	
	doc.module{
		name = "Image",
		path = repo_relative("/modules/image"),
		
		file = "src/lua_image.cpp",
	},
	
	doc.module{
		name = "video",
		path = repo_relative("/modules/video"),
		
		file = "src/lua_video.cpp",
	},
	
	---[[
	doc.module{
		name = "space",
		path = repo_relative("/modules/space"),
		
		file = "src/bindings/lua_space.cpp",
		
		files = {
			"src/bindings/space_nav.cpp",
			"src/bindings/space_space.cpp",
			"vec/vec3_udata.h",
			"vec/quat_udata.h",
		}
	},--]]
}


---[[
local options = {
	index_page = true,
	frames = true,
	image = script.path.."/../resources/images/LuaAV.96.png",
--	verbose = true,
}

doc.install_modules(
	script.path.."/../../extra/doc", 
	mods,
	options
)
--]]