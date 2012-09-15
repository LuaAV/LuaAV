Project{
	name = "libluaav",
	product_name = "luaav",
	type = platform.libtype.shared,
	path = Path{reporoot, "libluaav"},
	
	cflags = table.combine({
			"-DOSC_HOST_LITTLE_ENDIAN",
			"-D__STDC_CONSTANT_MACROS",
		},
		({
			[64] = {"-Dx86_64"},
		})[OSBIT],
		platform.cflags
	),
	
	ldflags = table.combine({
		},
		platform.so_ldflags,
		platform.ldflags
	),

	include_search_paths = table.combine({
		Path{reporoot, "dev/include"},
		Path{"include", relative = true},
		Path{"include/system", relative = true},
		Path{"include/types", relative = true},
		Path{"src", relative = true},
		Path{"../dev/include/Gamma", relative = true},
		Path{"src/oscpack/osc", relative = true},
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
			--Path{"/usr/include/lua5.1"},
			Path{"/usr/include/luajit-2.0"},
			Path{"/usr/include/apr-1.0"},
		--	Path{reporoot, "dev_linux/include"},
			--Path{"library/portaudio/src/os/unix", relative = true},
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
					name = "rt",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "apr-1",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "asound",
					path = "/usr/lib",
					type = "shared",
				},

				Dependency{
					name = "sndfile",
					path = "/usr/lib",
					type = "shared",
				},

			},
			WIN32 = {},
		})[OS]
	),
	
	src = table.combine({
		Path{"src/luaav.cpp", relative = true},
		Path{"src/luaav_audio.cpp", relative = true},
		Path{"src/luaav_audio_jit.cpp", relative = true},
		Path{"src/luaav_audio_reverb.cpp", relative = true},
		Path{"src/luaav_audio_soundfile.cpp", relative = true},
		Path{"src/luaav_scheduler.cpp", relative = true},
		Path{"src/luaav_midi.cpp", relative = true},
		Path{"src/luaav_osc.cpp", relative = true},
		Path{"src/lua_array.cpp", relative = true},

		Path{"src/lfs.cpp", relative = true},
		Path{"src/lpeg.c", relative = true},
		Path{"src/lpeg_list.cpp", relative = true},

		Path{"src/RtMidi.cpp", relative = true},

		Path{"allocore/al_pq.c", relative = true},

		Path{"allocore/system/al_Time.cpp", relative = true},
		Path{"allocore/system/al_Memory.cpp", relative = true},
		Path{"allocore/system/al_ThreadAPR.cpp", relative = true},
		Path{"allocore/protocol/al_Serialize.cpp", relative = true},
		Path{"allocore/types/al_MsgQueue.cpp", relative = true},
		Path{"allocore/types/al_Color.cpp", relative = true},

		Path{"jack/ringbuffer.c", relative = true},

		Path{"gamma/arr.cpp", relative = true},
		--Path{"gamma/AudioIO.cpp", relative = true},
		Path{"gamma/Conversion.cpp", relative = true},
		Path{"gamma/DFT.cpp", relative = true},
		Path{"gamma/FFT_fftpack.cpp", relative = true},
		Path{"gamma/fftpack++1.cpp", relative = true},
		Path{"gamma/fftpack++2.cpp", relative = true},
		Path{"gamma/File.cpp", relative = true},
		Path{"gamma/Print.cpp", relative = true},
		Path{"gamma/Recorder.cpp", relative = true},
		Path{"gamma/scl.cpp", relative = true},
		Path{"gamma/SoundFile.cpp", relative = true},
		Path{"gamma/Sync.cpp", relative = true},

		Path{"oscpack/osc/OscOutboundPacketStream.cpp", relative = true},
		Path{"oscpack/osc/OscPrintReceivedElements.cpp", relative = true},
		Path{"oscpack/osc/OscReceivedElements.cpp", relative = true},
		Path{"oscpack/osc/OscTypes.cpp", relative = true},
	}, {}
	),
	
	Debug = Config{
		cflags = platform.Debug.cflags,
		install_path = Path{"/usr/local/lib"},
	},
	
	Release = Config{
		cflags = platform.Release.cflags,
		install_path = Path{"/usr/local/lib"},
	},
}
