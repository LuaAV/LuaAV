Project{
	name = "color",
	type = "module",
	path = Path{modulesfolder, "color"},
	
	cflags = table.combine({
		--"-D__STDC_LIMIT_MACROS",
		},
		platform.cflags
	),	
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	

	include_search_paths = table.combine({
			Path{reporoot, "dev/include"},
			Path{"", relative = true},
			Path{"Module", relative = true},
		}, 

		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{reporoot, "dev/include"},
			},
			WIN32 = {},
		})[OS],

		platform.include_search_paths
	),
	
	linker_search_paths = {},
	
	dependencies = table.combine(
		{},
		({
			LINUX = {
				Dependency{
					name = "lua5.1",
					path = "/usr/lib",
					type = "shared",
				},
			}
		})[OS]
	),
	
	src = {
		Path{"cc.c", relative = true},
		Path{"lua_color.cpp", relative = true},
	},
	
	on_install = function(self, config)

	end,
	
	Debug = Config{
		cflags = platform.Debug.cflags,
		install_path = Path{reporoot, "sysbuild/Debug/modules"},
	},
	
	Release = Config{
		cflags = platform.Release.cflags,
		install_path = Path{reporoot, "sysbuild/Release/modules"},
	},
}
