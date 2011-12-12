Project{
	name = "csound",
	type = "module",
	path = Path{modulesfolder, "csound"},
	
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
			Path{"src", relative = true},
		}, 

		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{"/usr/include/csound"},
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

				Dependency{
					name = "csound64",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "csnd",
					path = "/usr/lib",
					type = "shared",
				},
			}
		})[OS]
	),
	
	src = {
		Path{"src/luaopen_csound.cpp", relative = true},
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
