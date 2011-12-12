Project{
	name = "opengl",
	type = "module",
	path = Path{modulesfolder, "opengl"},
	
	cflags = platform.cflags,
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{"include", relative = true},
			Path{"include/graphics", relative = true},
			Path{"include/math", relative = true},
			Path{"include/protocol", relative = true},
			Path{"include/types", relative = true},
			Path{"src/glew", relative = true},
			Path{"src/glew/GL", relative = true},
			Path{reporoot, "dev/include"},
		},
		
		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
			},
			WIN32 = {},
		})[OS],

		platform.include_search_paths
	),
	
	linker_search_paths = {},
	
	dependencies = table.combine({
		},
		platform.dependency.OpenGL
	),
	
	src = {
		Path{"src/lua_opengl.cpp", relative = true},
		Path{"src/lua_opengl_camera.cpp", relative = true},
		Path{"src/lua_opengl_extensions.cpp", relative = true},
		Path{"src/lua_opengl_model.cpp", relative = true},
		Path{"src/lua_opengl_mesh.cpp", relative = true},
		Path{"src/lua_opengl_shader.cpp", relative = true},
		Path{"src/lua_opengl_sketch.cpp", relative = true},
		Path{"src/lua_opengl_slab.cpp", relative = true},
		Path{"src/lua_opengl_tess.cpp", relative = true},
		Path{"src/lua_opengl_texture.cpp", relative = true},
		Path{"src/lua_openglu.cpp", relative = true},
	
		Path{"src/graphics/al_Mesh.cpp", relative = true},
		Path{"src/graphics/al_Model.cpp", relative = true},
		Path{"src/graphics/al_ShaderAttribute.cpp", relative = true},
		Path{"src/graphics/al_ShaderBackend.cpp", relative = true},
		Path{"src/graphics/al_ShaderBackendGLSL.cpp", relative = true},
		Path{"src/graphics/al_Shader.cpp", relative = true},
		Path{"src/graphics/al_ShaderParam.cpp", relative = true},
		Path{"src/graphics/al_ShaderProgram.cpp", relative = true},
		Path{"src/graphics/al_Slab.cpp", relative = true},
		Path{"src/graphics/al_Surface.cpp", relative = true},
		Path{"src/graphics/al_Texture.cpp", relative = true},

		Path{"src/protocol/al_Graphics.cpp", relative = true},
		Path{"src/protocol/al_GraphicsBackendOpenGL.cpp", relative = true},
		Path{"src/protocol/al_GraphicsBackendOpenGLPBO.cpp", relative = true},

		Path{"src/glew/glew.c", relative = true},

		Path{"src/math/al_Quat.cpp", relative = true},

		Path{"src/types/al_Color.cpp", relative = true},
	},
	
	
	Debug = Config{
		cflags = platform.Debug.cflags,
		install_path = Path{reporoot, "sysbuild/Debug/modules"},
	},
	
	Release = Config{
		cflags = platform.Release.cflags,
		install_path = Path{reporoot, "sysbuild/Release/modules"},
	},
}
