Project{
	name = "luasql",
	type = "module",
	path = Path{modulesfolder, "sql"},
	
	cflags = platform.cflags,
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{"src", relative = true},
		}, 
		platform.include_search_paths,
		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/mysql"},
				Path{"/usr/include/lua5.1"},
			},
			WIN32 = {}
		})[OS]
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
						name = "mysqlclient",
						path = "/usr/lib",
						type = "shared",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/luasql.c", relative = true},
		Path{"src/ls_mysql.c", relative = true},
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
