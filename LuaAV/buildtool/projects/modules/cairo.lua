Project{
	name = "lcairo",
	type = "module",
	path = Path{modulesfolder, "cairo"},
	
	cflags = platform.cflags,
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{"include", relative = true},
		},
		
		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{"/usr/include/cairo"},
				Path{"/usr/include/directfb"},
				Path{"/usr/include/freetype2"}
			},
			WIN32 = {},
		})[OS],

		platform.include_search_paths
	),
	
	linker_search_paths = {},
	
	dependencies = table.combine({
		},
		({
			OSX = {},
			LINUX = table.combine({
					Dependency{
						name = "lua5.1",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "cairo",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "directfb",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "freetype",
						path = "/usr/lib",
						type = "shared",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/lcairo.c", relative = true},
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
