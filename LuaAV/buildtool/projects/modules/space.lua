Project{
	name = "space",
	type = "module",
	path = Path{modulesfolder, "space"},
	
	cflags = table.combine(
		platform.cflags,
		({
			OSX = {},
			LINUX = {
			},
			WIN32 = {}
		})[OS]
	),
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{reporoot, "dev/include"},
			Path{"src", relative = true},
			Path{"vec", relative = true},
			Path{"src/neighand-0.2", relative = true},
			Path{"src/bindings", relative = true},
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
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/Nav.cpp", relative = true},
		Path{"src/quat.cpp", relative = true},
		Path{"src/Space.cpp", relative = true},

		Path{"vec/space_vec.cpp", relative = true},
		Path{"src/bindings/lua_space.cpp", relative = true},
		Path{"src/bindings/space_nav.cpp", relative = true},
		Path{"src/bindings/space_space.cpp", relative = true},
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
