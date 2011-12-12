Project{
	name = "mime",
	product_name = "core",
	type = "module",
	path = Path{modulesfolder, "socket"},
	
	cflags = platform.cflags,
	
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
			},
			WIN32 = {},
		})[OS],

		platform.include_search_paths
	),
	
	linker_search_paths = {},
	
	dependencies = table.combine({
		}
	),
	
	src = {
		Path{"src/mime.c", relative = true},
	},
	
	
	on_install = function(self, config)
		local scripts_path = Path{"src", relative = true}
		scripts_path:resolve(self.path[1])

		local modules_path = makepath(self[config].install_path[1], "mime")
		copyfile(makepath(scripts_path[1], "mime.lua"), 
								self[config].install_path[1])

		movefile(makepath(self[config].install_path[1], self.buildname), modules_path)
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
