Project{
	name = "Image",
	type = "module",
	path = Path{modulesfolder, "image"},
	
	cflags = platform.cflags,
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{reporoot, "dev/include"},
			Path{"src", relative = true},
			Path{"src/image", relative = true},
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
		platform.dependency.OpenGL,
		({
			OSX = {},
			LINUX = table.combine({
					Dependency{
						name = "lua5.1",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "freeimage",
						path = "/usr/lib",
						type = "shared",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/lua_image.cpp", relative = true},
		Path{"src/image/FreeImageImpl.cpp", relative = true},
		Path{"src/image/Image.cpp", relative = true},
		Path{"src/image/ImageImpl.cpp", relative = true},
		Path{"src/image/RGBImageImpl.cpp", relative = true},
		Path{"src/image/rgb_io.cpp", relative = true},
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
