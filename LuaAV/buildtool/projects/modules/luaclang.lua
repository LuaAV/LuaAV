Project{
	name = "clang",
	type = "module",
	path = Path{modulesfolder, "luaclang"},
	
	cflags = table.combine({
			"-D__STDC_LIMIT_MACROS",
			"-D__STDC_CONSTANT_MACROS",
			"-fno-rtti",
			"-fPIC",
			"-fvisibility=default",
			"-rdynamic",
		},
		platform.cflags
	),	
	
	ldflags = table.combine({
			"--whole-archive",
			--"-E",
			--"--export_dynamic", 
			--"--export-all-symbols"
		},
		platform.so_ldflags,
		platform.ldflags
	),
	

	include_search_paths = table.combine({
			Path{"src", relative = true},
			Path{reporoot, "dev/include"},
		}, 

		({
			OSX = {
				Path{"llvm-2.8/include", relative = true},
			},
			LINUX = {
				Path{"llvm-2.8/include_linux" .. ((OSBIT==64 and "64") or ""), relative = true},
				Path{"/usr/include/lua5.1"},
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
					path = "/usr/lib/llvm-2.8/lib",
					type = "shared",
				},

--Order of libraries is important:
				Dependency{
					name = "clangIndex",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangChecker",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangFrontend",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangDriver",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangBasic",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangLex",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangParse",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangSema",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangAST",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangRewrite",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangSerialization",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangAnalysis",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "clangBasic",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},


-- LLVM
				
				Dependency{
					name = "LLVMX86AsmParser",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMX86AsmPrinter",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
				
				Dependency{
					name = "LLVMX86CodeGen",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
				
				Dependency{
					name = "LLVMX86Info",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},


				Dependency{
					name = "LLVMLinker",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMipo",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMInstrumentation",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMJIT",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMExecutionEngine",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
--[[
				Dependency{
					name = "LLVMDebugger",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
--]]		
				Dependency{
					name = "LLVMBitWriter",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},







				Dependency{
					name = "LLVMX86AsmParser",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMArchive",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMBitReader",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},




				Dependency{
					name = "LLVMSelectionDAG",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
				
				Dependency{
					name = "LLVMAsmPrinter",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMCodeGen",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMScalarOpts",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
--[[
				Dependency{
					name = "LLVMInstCombine",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
--]]
				Dependency{
					name = "LLVMTransformUtils",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMipa",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
			
				Dependency{
					name = "LLVMAnalysis",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMTarget",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMMC",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},



				
				Dependency{
					name = "LLVMCore",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMSupport",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMSystem",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},
				
				


--[[


				Dependency{
					name = "LLVMInterpreter",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMX86Disassembler",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMMCParser",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},

				Dependency{
					name = "LLVMAsmParser",
					path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "")),
					type = "static",
				},--]]
			}
		})[OS]
	),
	
	src = {
		Path{"src/lua_clang.cpp", relative = true},
		Path{"src/al_Compiler.cpp", relative = true},
	},
	
--[[
	on_install = function(self, config)

		-- copy Headers
		local header_path = makepath(self[config].install_path[1], "../Headers")
		local include_path = makepath(modulesfolder, "luaclang/llvm-2.8/lib_linux" .. ((OSBIT==64 and "64") or "") .. "/clang/2.8/include")
		
		copyfile(makepath(include_path, "emmintrin.h"), header_path)
		copyfile(makepath(include_path, "float.h"), header_path)
		copyfile(makepath(include_path, "iso646.h"), header_path)
		copyfile(makepath(include_path, "limits.h"), header_path)
		copyfile(makepath(include_path, "mmintrin.h"), header_path)
		copyfile(makepath(include_path, "mm_malloc.h"), header_path)
		copyfile(makepath(include_path, "pmmintrin.h"), header_path)
		copyfile(makepath(include_path, "stdarg.h"), header_path)
		copyfile(makepath(include_path, "stdbool.h"), header_path)
		copyfile(makepath(include_path, "stddef.h"), header_path)
		copyfile(makepath(include_path, "stdint.h"), header_path)
		copyfile(makepath(include_path, "tgmath.h"), header_path)
		copyfile(makepath(include_path, "tmmintrin.h"), header_path)
		copyfile(makepath(include_path, "xmmintrin.h"), header_path)
	end,
	--]]

	Debug = Config{
		cflags = platform.Debug.cflags,
		install_path = Path{reporoot, "sysbuild/Debug/modules"},
	},
	
	Release = Config{
		cflags = platform.Release.cflags,
		install_path = Path{reporoot, "sysbuild/Release/modules"},
	},
}
