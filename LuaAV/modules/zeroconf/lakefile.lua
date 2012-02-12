
local src = {
	"luaopen_zeroconf.cpp",
}

if PLAT == "Darwin" then
	src[#src+1] = "al_Zeroconf_OSX.mm"
else
	src[#src+1] = "al_Zeroconf.cpp"
end

cpp.shared{
	"zeroconf",
	src = src,
	needs = {
		"lua",
	},
}
