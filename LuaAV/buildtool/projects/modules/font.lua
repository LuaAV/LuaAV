Project{
	name = "font",
	type = "module",
	path = Path{modulesfolder, "font"},
	
	cflags = table.combine({
			"-DFONT_QT",		
		},		
		platform.cflags
	),

	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{reporoot, "dev/include"},
			Path{"src", relative = true},
		}, 

		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{"/usr/include/freetype2"},
				Path{"/usr/include/qt4"},
				Path{"/usr/include/qt4/QtCore"},
				Path{"/usr/include/qt4/QtGui"},
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
						name = "freetype",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "QtGui",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "QtCore",
						path = "/usr/lib",
						type = "shared",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/Font.cpp", relative = true},
		Path{"src/lua_font_udata.cpp", relative = true},
		Path{"src/lua_font.cpp", relative = true},
		Path{"src/font_freetype.cpp", relative = true},
		Path{"src/font_qt.cpp", relative = true},
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
