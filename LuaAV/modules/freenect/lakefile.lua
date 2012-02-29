

-- Lake file for luaclang
USB_1_0_LIBS = 'freenect'
USB_1_0_INCLUDE_DIR = 'freenect'

FREENECT_LIBS = 'freenect'
FREENECT_INCLUDE_DIR = 'freenect'

if PLAT == "Darwin" then

elseif PLAT == "Linux" then
	
else
	error (PLAT .. " not supported")
end

cpp.shared{ 
	'freenect', 
	-- source files:
	src = {
		'luaopen_freenect.cpp', 
	},
	--defines = {},
	--flags = "-fno-rtti -fomit-frame-pointer -fno-exceptions -fPIC -O3",
	-- library dependencies:
	needs = { "freenect", "usb-1.0", "lua" },
}
