Project{
	name = "kinect",
	type = "module",
	path = Path{modulesfolder, "kinect"},
	
	cflags = platform.cflags,
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{"libfreenect/include", relative = true},
			Path{reporoot, "dev/include"},
		},
		
		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{"/usr/include/libusb-1.0"},
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
						name = "usb-1.0",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "freenect",
						path = makepath(modulesfolder, "kinect/libfreenect/lib_linux"),
						type = "static",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/luaopen_kinect.cpp", relative = true},
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
