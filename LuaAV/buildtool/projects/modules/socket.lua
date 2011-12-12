Project{
	name = "socket",
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
		Path{"src/auxiliar.c", relative = true},
		Path{"src/buffer.c", relative = true},
		Path{"src/except.c", relative = true},
		Path{"src/inet.c", relative = true},
		Path{"src/io.c", relative = true},
		Path{"src/luasocket.c", relative = true},
		Path{"src/options.c", relative = true},
		Path{"src/select.c", relative = true},
		Path{"src/tcp.c", relative = true},
		Path{"src/timeout.c", relative = true},
		Path{"src/udp.c", relative = true},
		Path{"src/unix.c", relative = true},
		Path{"src/usocket.c", relative = true},
	},
	
	
	on_install = function(self, config)
		local scripts_path = Path{"src", relative = true}
		scripts_path:resolve(self.path[1])

		local modules_path = makepath(self[config].install_path[1], "socket")
		copyfile(makepath(scripts_path[1], "socket.lua"), 
								self[config].install_path[1])

		movefile(makepath(self[config].install_path[1], self.buildname), modules_path)
		
		ensurepath(modules_path)
		copyfile(makepath(scripts_path[1], "ftp.lua"), modules_path)
		copyfile(makepath(scripts_path[1], "http.lua"), modules_path)
		copyfile(makepath(scripts_path[1], "ltn12.lua"), modules_path)
		copyfile(makepath(scripts_path[1], "smtp.lua"), modules_path)
		copyfile(makepath(scripts_path[1], "socket.lua"), modules_path)	
		copyfile(makepath(scripts_path[1], "tp.lua"), modules_path)
		copyfile(makepath(scripts_path[1], "url.lua"), modules_path)
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
