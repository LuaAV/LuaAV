Project{
	name = "video",
	type = "module",
	path = Path{modulesfolder, "video"},
	
	cflags = table.combine(
		platform.cflags,
		({
			OSX = {},
			LINUX = {
			--	"-DTARGET_LINUX",
				"-DOF_VIDEO_PLAYER_GSTREAMER",
				"-DOF_VIDEO_CAPTURE_GSTREAMER",
				"-DOF_GST_USING_HAL",
				"-DVIDEO_USE_GST",
			},
			WIN32 = {}
		})[OS]
	),
	
	ldflags = table.combine(
		platform.so_ldflags,
		platform.ldflags
	),
	
	include_search_paths = table.combine({
			Path{reporoot, "dev/include"},
			Path{"of", relative = true},
			Path{"src", relative = true},
			Path{"src/video", relative = true},
		}, 

		({
			OSX = {},
			LINUX = {
				Path{"/usr/include/lua5.1"},
				Path{"/usr/include/gstreamer-0.10"},
				Path{"/usr/include/glib-2.0"},
				Path{"/usr/lib/glib-2.0/include"},
				Path{"/usr/include/libxml2"},
				Path{"/usr/include/hal"},
				Path{"/usr/include/dbus-1.0"},
				Path{"/usr/lib/dbus-1.0/include"},
			},
			WIN32 = {},
		})[OS],

		platform.include_search_paths
	),
	
	linker_search_paths = {},
	
	dependencies = table.combine({
		},
		platform.dependency.OpenGL,
		platform.dependency.gstreamer,
		({
			OSX = {},
			LINUX = table.combine({
					Dependency{
						name = "lua5.1",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "gstapp-0.10",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "udev",
						path = "/usr/lib",
						type = "shared",
					},

					Dependency{
						name = "hal",
						path = "/usr/lib",
						type = "shared",
					},
				}),
			WIN32 = {},
		})[OS]
	),
	
	src = {
		Path{"src/lua_video.cpp", relative = true},

		Path{"src/video/VideoCamera.cpp", relative = true},
		Path{"src/video/VideoCameraImpl.cpp", relative = true},
		Path{"src/video/ofVideoCameraImpl.cpp", relative = true},

		Path{"src/video/Video.cpp", relative = true},
		Path{"src/video/VideoImpl.cpp", relative = true},
		Path{"src/video/GstVideoImpl.cpp", relative = true},
		Path{"src/video/GstUtils.cpp", relative = true},

		Path{"of/ofGstUtils.cpp", relative = true},
		Path{"of/ofGraphics.cpp", relative = true},
		Path{"of/ofVideoGrabber.cpp", relative = true},
	--	Path{"of/ofVideoPlayer.cpp", relative = true},
	--	Path{"src/video/VideoRecorder.cpp", relative = true},
	--	Path{"src/video/VideoRecorderImpl.cpp", relative = true},
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
