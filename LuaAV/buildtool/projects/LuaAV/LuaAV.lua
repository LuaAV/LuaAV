Project{
	name = "LuaAV",
	product_name = "LuaAV",
	type = "application",
	path = Path{reporoot, "LuaAVApp"},	
	
	cflags = table.combine({
			"-DLUAAV_APP_USE_QT",
--			"-DPA_USE_OSS",
			"-DPA_USE_JACK",
			"-DPA_USE_ALSA",
			"-DHAVE_SYS_SOUNDCARD_H",

			-- for clang/LLVM:
			"-D__STDC_LIMIT_MACROS",
			"-D__STDC_CONSTANT_MACROS",
			"-fno-rtti",
		},
		platform.cflags
	),
	
	ldflags = table.combine({
		},
		platform.ldflags
	),

---[[
	preprocess = function(self)
		-- Qt "moc" command
		local moc_cmd = "/usr/bin/moc-qt4 -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 %s -o %s"

		-- files to "moc"
		local paths = {
			Path{"linux/luaav_console_qt.h", relative = true},
			Path{"linux/luaav_app_menu_qt.h", relative = true},
			Path{"linux/al_mainloop_qt.h", relative = true},
			Path{"linux/luaav_app_window_opengl_qt.h", relative = true},
			Path{"linux/luaav_app_codepad_qt.h", relative = true},
			Path{"linux/luaav_app_codepad_window_qt.h", relative = true},
			Path{"linux/luaav_app_codepad_highlighter_qt.h", relative = true},
			Path{"linux/stateview/luaav_stateview_model.h", relative = true},
			Path{"linux/stateview/luaav_stateview_qt.h", relative = true},
		}

		for i, p in ipairs(paths) do
			p:resolve(self.path[1])
		
			-- "moc" output
			local path = buildutils.stripfilename(p[1])
			local filename = buildutils.strippath(p[1])
			local stem = buildutils.stripextension(filename)
			local moc_filename = string.format("%s/moc_%s.cpp", path, stem)
		
			-- shell command
			local cmd = string.format(moc_cmd, p[1], moc_filename)
			buildutils.exec(cmd)
		end
	end,
--]]	
	include_search_paths = table.combine({
		Path{reporoot, "dev/include"},
		Path{"include", relative = true},
		Path{"src", relative = true},
		Path{reporoot, "dev/include"},
		Path{reporoot, "dev/include/system"},
		Path{reporoot, "dev/include/types"},

		-- clang/LLVM
		Path{"luaclang/llvm-2.8/include", relative = true},	
	}, ({
		--[[
		OSX = {
			Path{reporoot, "dev_osx/include"},
			Path{"library/portaudio/src/hostapi/coreaudio", relative = true},
			Path{"library/portaudio/src/os/unix", relative = true},
			Path{"library/osx", relative = true},
			Path{"library/osx/UKKQueue", relative = true},
		},
		--]]

		LINUX = {
			Path{"/usr/include/lua5.1"},

			-- Filewatching			
			Path{"linux/inotify-tools", relative = true},
			Path{"linux/inotify-tools/libnotifytools/src", relative = true},
			Path{"linux/inotify-tools/libnotifytools/src/inotifytools", relative = true},
			Path{"linux/inotify-tools/src", relative = true},

			-- Portaudio
			--Path{"portaudio/include", relative = true},
			--Path{"portaudio/src/common", relative = true},
			--Path{"portaudio/src/os/unix", relative = true},

			-- Qt
			Path{"/usr/include/qt4"},
			Path{"/usr/include/qt4/QtCore"},
			Path{"/usr/include/qt4/QtGui"},
			Path{"/usr/include/qt4/QtOpenGL"},

			Path{"linux", relative = true},
			Path{"linux/stateview", relative = true},
	
		},
		--[[		
		WIN32 = {
			Path{"library/portaudio/src/os/win", relative = true},
		},
		--]]
	})[OS]),
	
	linker_search_paths = {},
	
	dependencies = table.combine({
		},
		platform.dependency.OpenGL,
		({
			OSX = {},
			LINUX = {		
				--[[Dependency{
					name = "lua5.1",
					path = "/usr/lib",
					type = "shared",
				},--]]
				Dependency{
					name = "luajit-5.1",
					path = "/usr/lib/x86_64-linux-gnu/libluajit-5.1.a",
					type = "shared",
				},

				Dependency{
					name = "luaav",
					path = "/usr/local/lib",
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

				Dependency{
					name = "QtOpenGL",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "pthread",
					path = "/usr/lib",
					type = "shared",
				},
	
				Dependency{
					name = "Xxf86vm",
					path = "/usr/lib",
					type = "shared",
				},
	
				Dependency{
					name = "rt",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "jack",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "asound",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "portaudio",
					path = "/usr/lib",
					type = "shared",
				},

				-- Clang/LLVM:

				Dependency{
					name = "LLVMJIT",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "EnhancedDisassembly",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clang",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86AsmParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInstrumentation",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86Disassembler",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangFrontendTool",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangCodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "CompilerDriver",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangChecker",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangRewrite",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangFrontend",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangDriver",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangParse",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangSerialization",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangFrontend",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangSema",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangIndex",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangLex",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangAnalysis",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangAST",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangBasic",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMpic16passes",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMAsmParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LTO",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMLinker",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCBackend",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCBackendInfo",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86CodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86AsmPrinter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86Info",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMAsmPrinter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMMCParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMBitWriter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMipo",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMArchive",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMBitReader",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMSelectionDAG",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInterpreter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMExecutionEngine",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMScalarOpts",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInstCombine",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMTransformUtils",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMipa",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMAnalysis",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMTarget",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCore",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMMC",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMSupport",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMSystem",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

--[[
				Dependency{
					name = "clangFrontend",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangBasic",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangCodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangParse",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangLex",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangAST",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},




				Dependency{
					name = "LLVMJIT",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},


				Dependency{
					name = "LLVMExecutionEngine",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMScalarOpts",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInstCombine",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMTransformUtils",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

--Order of libraries is important:
				Dependency{
					name = "clangIndex",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangChecker",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangDriver",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangLex",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangParse",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangSema",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangAST",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangRewrite",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangSerialization",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "clangAnalysis",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},


-- LLVM
				
				Dependency{
					name = "LLVMX86AsmParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86AsmPrinter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
				
				Dependency{
					name = "LLVMX86CodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
				
				Dependency{
					name = "LLVMX86Info",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},


				Dependency{
					name = "LLVMLinker",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMipo",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInstrumentation",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},



				Dependency{
					name = "LLVMDebugger",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
		
				Dependency{
					name = "LLVMBitWriter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},







				Dependency{
					name = "LLVMX86AsmParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMArchive",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMBitReader",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},




				Dependency{
					name = "LLVMSelectionDAG",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
				
				Dependency{
					name = "LLVMAsmPrinter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCodeGen",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMScalarOpts",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMInstCombine",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMTransformUtils",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMipa",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
			
				Dependency{
					name = "LLVMAnalysis",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},




				
				Dependency{
					name = "LLVMAsmParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				





				Dependency{
					name = "LLVMInterpreter",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMX86Disassembler",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMMCParser",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
				Dependency{
					name = "LLVMMC",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},



				Dependency{
					name = "LLVMTarget",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMCore",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMSupport",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},

				Dependency{
					name = "LLVMSystem",
					path = reporoot .. "/LuaAVApp/luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or ""),
					type = "static",
				},
				--]]
			},
			WIN32 = {},
		})[OS]
	),
	
	src = table.combine({
		Path{"src/luaav_app.cpp", relative = true},
		Path{"src/al_mainloop.cpp", relative = true},
		Path{"src/luaav_filewatcher.cpp", relative = true},
		Path{"src/luaav_app_audiodriver.cpp", relative = true},
		Path{"src/luaav_app_menu.cpp", relative = true},
		Path{"src/luaav_app_window.cpp", relative = true},
		Path{"src/luaav_app_codepad.cpp", relative = true},

		Path{"luaclang/src/lua_clang.cpp", relative = true},
		Path{"luaclang/src/al_Compiler.cpp", relative = true},
	},
		({
		OSX = {
			Path{"library/lua-5.1.4/src/all.c", relative = true},
			Path{"library/util/dlmalloc.c", relative = true},
			Path{"library/src/LuaAVAudioUnit.cpp", relative = true},
			

			Path{"library/osx/LuaAVConsole.mm", relative = true},
			Path{"library/osx/LuaAVFileWatcherCocoa.mm", relative = true},
			Path{"library/osx/LuaAVLog.mm", relative = true},
			Path{"library/osx/LuaAVMenuCocoa.mm", relative = true},
			Path{"library/osx/LuaAVOSX.mm", relative = true},
			Path{"library/osx/LuaAVOSXClock.mm", relative = true},
			Path{"library/osx/LuaAVScreens.mm", relative = true},
			Path{"library/osx/LuaAVWindowCocoa.mm", relative = true},
			Path{"library/osx/UKKQueue/UKFileWatcher.m", relative = true},
			Path{"library/osx/UKKQueue/UKFNSubscribeFileWatcher.m", relative = true},
			Path{"library/osx/UKKQueue/UKKQueue.m", relative = true},
			Path{"library/osx/UKKQueue/UKMainThreadProxy.m", relative = true},
			
			Path{"library/portaudio/src/hostapi/coreaudio/pa_mac_core.c", relative = true},
			Path{"library/portaudio/src/hostapi/coreaudio/pa_mac_core_blocking.c", relative = true},
			Path{"library/portaudio/src/hostapi/coreaudio/pa_mac_core_utilities.c", relative = true},
			Path{"library/portaudio/src/os/mac_osx/pa_mac_hostapis.c", relative = true},
			
			
			Path{"library/luamidi/lua.midi.cpp", relative = true},
			Path{"library/luamidi/RtMidi.cpp", relative = true},

			Path{"library/util/dfx-au-utilities.c", relative = true},
		},
		
		LINUX = {
			-- App (generic Linux)
			Path{"linux/luaav_app_log_linux.cpp", relative = true},

			-- App (Qt)
			Path{"linux/luaav_main_qt.cpp", relative = true},			
			Path{"linux/luaav_app_qt.cpp", relative = true},
			Path{"linux/al_mainloop_qt.cpp", relative = true},
			Path{"linux/moc_al_mainloop_qt.cpp", relative = true},
			Path{"linux/luaav_console_qt.cpp", relative = true},
			Path{"linux/moc_luaav_console_qt.cpp", relative = true},
			Path{"linux/luaav_app_menu_qt.cpp", relative = true},
			Path{"linux/moc_luaav_app_menu_qt.cpp", relative = true},
			Path{"linux/luaav_app_window_qt.cpp", relative = true},
			Path{"linux/luaav_app_window_opengl_qt.cpp", relative = true},
			Path{"linux/moc_luaav_app_window_opengl_qt.cpp", relative = true},
			Path{"linux/luaav_app_codepad_qt.cpp", relative = true},	
			Path{"linux/moc_luaav_app_codepad_qt.cpp", relative = true},
			Path{"linux/luaav_app_codepad_window_qt.cpp", relative = true},	
			Path{"linux/moc_luaav_app_codepad_window_qt.cpp", relative = true},
			Path{"linux/luaav_app_codepad_highlighter_qt.cpp", relative = true},	
			Path{"linux/moc_luaav_app_codepad_highlighter_qt.cpp", relative = true},

			Path{"src/luaav_app_stateview.cpp", relative = true},
			Path{"linux/stateview/luaav_stateview_item.cpp", relative = true},
			Path{"linux/stateview/luaav_stateview_model.cpp", relative = true},	
			Path{"linux/stateview/moc_luaav_stateview_model.cpp", relative = true},
			Path{"linux/stateview/luaav_stateview_qt.cpp", relative = true},	
			Path{"linux/stateview/moc_luaav_stateview_qt.cpp", relative = true},

			-- App (X11)
--			Path{"linux/luaav_app_window_x11.cpp", relative = true},
			Path{"linux/luaav_app_screens_x11.cpp", relative = true},

			-- Filewatching
			Path{"linux/luaav_filewatcher_linux.cpp", relative = true},
			Path{"linux/inotify-tools/libnotifytools/src/inotifytools.cpp", relative = true},
			Path{"linux/inotify-tools/libnotifytools/src/redblack.cpp", relative = true},
			Path{"linux/inotify-tools/src/common.cpp", relative = true},
			Path{"linux/inotify-tools/src/inotifywait.cpp", relative = true},
			Path{"linux/inotify-tools/src/inotifywatch.cpp", relative = true},

			-- Portaudio
			--[[
			Path{"portaudio/src/common/pa_allocation.c", relative = true},
			Path{"portaudio/src/common/pa_converters.c", relative = true},
			Path{"portaudio/src/common/pa_cpuload.c", relative = true},
			Path{"portaudio/src/common/pa_debugprint.c", relative = true},
			Path{"portaudio/src/common/pa_dither.c", relative = true},
			Path{"portaudio/src/common/pa_front.c", relative = true},
			Path{"portaudio/src/common/pa_process.c", relative = true},
			Path{"portaudio/src/common/pa_ringbuffer.c", relative = true},
			Path{"portaudio/src/common/pa_skeleton.c", relative = true},
			Path{"portaudio/src/common/pa_stream.c", relative = true},
			Path{"portaudio/src/common/pa_trace.c", relative = true},

			Path{"portaudio/src/os/unix/pa_unix_hostapis.c", relative = true},
			Path{"portaudio/src/os/unix/pa_unix_util.c", relative = true},

			Path{"portaudio/src/hostapi/jack/pa_jack.c", relative = true},
			Path{"portaudio/src/hostapi/oss/pa_unix_oss.c", relative = true},
			Path{"portaudio/src/hostapi/alsa/pa_linux_alsa.c", relative = true},
			--]]
		},
		
		WIN32 = {
			Path{"library/windows/LuaAVConsole.cpp", relative = true},
			Path{"library/windows/LuaAVEventLoopWindows.cpp", relative = true},
			Path{"library/windows/LuaAVFileWatcherWindows.cpp", relative = true},
			Path{"library/windows/LuaAVMenuWindows.cpp", relative = true},
			Path{"library/windows/LuaAVScreens.cpp", relative = true},
			Path{"library/windows/LuaAVWindow.cpp", relative = true},
			Path{"library/windows/LuaAVWindows.cpp", relative = true},
			Path{"library/windows/LuaAVWindowsClock.cpp", relative = true},
			Path{"library/windows/LuaAVWindowWindows.cpp", relative = true},
		},
	})[OS]
	--]]
	),

	on_install = function(self, config)
		-- copy script resources to install directory
		local resource_path = makepath(self[config].install_path[1], "resources")
		exportdir(makepath(self.path[1], "scripts"), resource_path)

		local header_path = makepath(self[config].install_path[1], "resources/Headers")

		local clang_path = Path{
			({
				[32] = "luaclang/llvm-2.8/lib_linux",
				[64] = "luaclang/llvm-2.8/lib_linux64",
			})[OSBIT], 
			relative = true
		}
		clang_path:resolve(self.path[1])
		exportdir(makepath(clang_path[1], "clang/2.8/include"), header_path)
		exportdir(makepath(reporoot, "dev/include"), header_path)

		local lua_include = Path{reporoot, "dependencies/lua-5.1.4/src"}
		copyfile(makepath(lua_include[1], "luaconf.h"), header_path)
		copyfile(makepath(lua_include[1], "lua.h"), header_path)
		copyfile(makepath(lua_include[1], "lualib.h"), header_path)
		copyfile(makepath(lua_include[1], "lauxlib.h"), header_path)
		copyfile(makepath(Path{reporoot, "dependencies/lua-5.1.4/etc"}[1], "lua.hpp"), header_path)
---[=[		

		-- copy icon to /usr/share/luaav
		local icon_path = Path{"linux", relative = true}
		icon_path:resolve(self.path[1])

		ensurepath("/usr/share/luaav", "sudo")
		copyfile(makepath(icon_path[1], "luaav3.512.png"), "/usr/share/luaav", "sudo")
		copyfile(makepath(icon_path[1], "eye.png"), "/usr/share/luaav", "sudo")
		copyfile(makepath(icon_path[1], "reload.png"), "/usr/share/luaav", "sudo")
		copyfile(makepath(icon_path[1], "x.png"), "/usr/share/luaav", "sudo")

		-- copy Lua modules
		local module_path = makepath(self[config].install_path[1], "modules")
		local extra_path = Path{reporoot, "extra"}
		extra_path:resolve(self.path[1])

		exportdir(makepath(extra_path[1], "modules"), module_path)
--]=]
	end,
	
	
	Debug = Config{
		cflags = platform.Debug.cflags,
		install_path = Path{reporoot, "sysbuild/Debug"},
	},
	
	Release = Config{
		cflags = platform.Release.cflags,
		install_path = Path{reporoot, "sysbuild/Release"},
	},
}
