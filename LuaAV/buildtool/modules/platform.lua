--[[
	Contains general plaform-specific information for describing projects such as:

		- how libraries and archives are described
		- standard compilation and linking flags
			- for both Debug and Release implementations
		- common search paths for compilation and linking
		- common system dependncy listings (e.g. OpenGL, gstreamer, ...) 
--]]
local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local next = next
local print = print
local setmetatable = setmetatable

local table = require 'table'
local string = require 'string'
local os = require 'os'
local io = require 'io'
local print = print

local Dependency = require("dependency")
local Path = require("path")

local OS = _OSNAME

local platform_data = {
	OSX = {
		-------------------------------------
		-- System info
		libtype = {
			shared = "framework",
		},
		
		extension = {
			module = "so",
			framework = "framework",
			static = "a",
		},

		prefix = {
			static = "lib",
		},

		-------------------------------------
		-- Flags
		cflags = {
			"-arch i386",
			"-isysroot /Developer/SDKs/MacOSX10.4u.sdk",
			"-mmacosx-version-min=10.3",
			"-fmessage-length=0",
			"-pipe",
			"-fPIC",
			"-Wno-trigraphs",
			"-fpascal-strings",
			"-fasm-blocks",
			"-Wreturn-type",
			"-Wunused-variable",
		},
		
		ldflags = {
			"-arch i386",
			"-mmacosx-version-min=10.3",
			"-undefined dynamic_lookup",
			"-isysroot /Developer/SDKs/MacOSX10.4u.sdk",
		},

		so_ldflags = {
			"-bundle"
		},

		-------------------------------------
		-- Include search paths
		include_search_paths = {
			Path{reporoot, "dev_osx/include"},
		},
		
		-------------------------------------
		-- Dependencies
		dependency = {
			OpenGL = {
				Dependency{
					name = "OpenGL",
					path = "/Developer/SDKs/MacOSX10.4u.sdk/Library/Frameworks",
					type = "framework",
				}
			}
		},
		
		-------------------------------------
		-- Configs
		Debug = {
			cflags = {
				"-O0",
				"-gdwarf-2",
				"-mfix-and-continue",
			},
			
			ldflags = {},
		},
		
		Release = {
			cflags = {
				" -Os",
			},
			
			ldflags = {},
		},
	},
-----------------------------------------------------------------------------------
	LINUX = {
		-------------------------------------
		-- System info
		libtype = {
			shared = "shared",
		},
		
		extension = {
			module = "so",
			shared = "so",
			static = "a",
		},

		prefix = {
			static = "lib",
		},

		-------------------------------------
		-- Dependencies
		dependency = {
			OpenGL = {
				Dependency{
					name = "GL",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "GLU",
					path = "/usr/lib",
					type = "shared",
				},
			},

			Cg = {
				Dependency{
					name = "Cg",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "CgGL",
					path = "/usr/lib",
					type = "shared",
				},
			},

			ffmpeg = {
				Dependency{
					name = "avutil",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "avformat",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "avcodec",
					path = "/usr/lib",
					type = "shared",
				},
			},
---[[
			gstreamer = {
				---[[
				Dependency{
					name = "gstbase-0.10",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "gstreamer-0.10",
					path = "/usr/lib",
					type = "shared",
				},
				--]]
				--[[
				Dependency{
					name = "gstbase-0.10-dbg",
					path = "/usr/lib/debug/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "gstreamer-0.10-dbg",
					path = "/usr/lib/debug/usr/lib",
					type = "shared",
				},
				--]]

				Dependency{
					name = "gstvideo-0.10",
					path = "/usr/lib",
					type = "shared",
				},
			},
--]]
		},

		-------------------------------------
		-- Flags
		cflags = {
			"-g",
			"-fmessage-length=0",
			"-pipe",
			"-fPIC",
			"-Wno-trigraphs",
			"-Wreturn-type",
			"-Wunused-variable",
			"-D__LINUX__",
		},
		
		ldflags = {},

		so_ldflags = {
			"-shared"
		},

		-------------------------------------
		-- Include search paths
		include_search_paths = {
			Path{reporoot, "dev_linux/include"},
		},

		-------------------------------------
		-- Local lib path
		local_lib_path = ({
			[32] = "dev_linux/lib",
			[64] = "dev_linux/lib64",
		})[_OSBIT],

		-------------------------------------
		-- Configs
		Debug = {
			cflags = {
				"-O0",
			},
			
			ldflags = {},
		},
		
		Release = {
			cflags = {
				"-O3",
			},
			
			ldflags = {},
		},
	},
-----------------------------------------------------------------------------------	
	WIN32 = {
		-------------------------------------
		-- System info
		libtype = {
			shared = "dll",
		},
		
		extension = {
			module = "dll",
		},
	},
}

-- makes it so that indexing the platform module automatically indexes
-- the appropriate platform-specific table of data
local
function makemeta(m)
	setmetatable(m, m)
	function m.__index(k, v)
		return platform_data[OS][v]
	end
end

module('platform', makemeta)
----------------------------------------------------------

