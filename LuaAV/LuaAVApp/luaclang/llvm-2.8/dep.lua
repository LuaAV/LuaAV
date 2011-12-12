
require "lfs"

local query = "__ZN4llvm4errsEv" --IntelInstPrinterE"

function shell(cmd)
	--print(cmd)
	local file = io.popen(cmd)
	local res = {}
	for v in file:lines() do table.insert(res, v) end
	return res
end

local pwd = unpack(shell("pwd")) .. "/lib_linux"

local libs = {}

---[[
for filename in lfs.dir(pwd) do
	if (filename:sub(1, 3) == "lib") then
		local libpath = pwd .. "/" .. filename
print("checking", libpath)
		local lib = { 
			path = libpath, 
			filename = filename, 
			t = {}, s = {}, d = {}, b = {},	-- local symbols
			T = {}, S = {}, D = {}, B = {},	-- external symbols
			U = {},	-- undefined symbols
			deps = {},	-- libraries this library needs
			used = {}, -- libraries that use this library
		}	
		local nms = shell(string.format("nm %s", libpath))
		for i = 3, #nms do
			local s = nms[i]
			for address, mode, symbol in string.gmatch(s, "([^%s]+) ([^%s]+) ([^%s]+)") do
				if not lib[mode] then
					--print(address, mode, symbol)	
				else			
lib[mode][symbol] = address
				end
			end
			for symbol in string.gmatch(s, "[%s]+U ([^%s]+)") do
				lib.U[symbol] = true
			end
		end
		libs[filename] = lib
	end
end

-- remove any undefined symbols that are defined in the same lib:
for k, lib in pairs(libs) do
	for u in pairs(lib.U) do
		if lib.T[u] or lib.D[u] or lib.t[u] or lib.d[u]
			or lib.s[u] or lib.b[u] or lib.S[u] or lib.B[u] then
			lib.U[u] = nil
		end
	end
end

---[[
-- now we can search for a given symbol:
function find(query, skip)
	local matches = {}
	for k, lib in pairs(libs) do
		if k ~= skip then
			local v = lib.T[query] or lib.D[query] or lib.S[query] or lib.B[query]
			if v then 
				table.insert(matches, k)
				break
			end
		end
	end
	return matches
end

-- look up all undefined symbols:
for k, lib in pairs(libs) do
	for u in pairs(lib.U) do
		local sources = find(u, k)
		if #sources > 0 then
			for i, v in ipairs(sources) do
				lib.deps[v] = true
				libs[v].used[k] = true
			end
		else
			--print("undefined:", k, u)
		end
	end
end

-- print out dependencies:
for k, lib in pairs(libs) do
	for u in pairs(lib.deps) do
		print(k, "needs", u)
	end
	for u in pairs(lib.used) do
		print(k, "used by", u)
	end
end
--]]

-- sort libs:

local out = {}

function rank(filename)
	for i, v in ipairs(out) do
		if v==filename then
			return i
		end
	end
	error(filename)
end

local memo = {}
local placed = {}

function place(filename)
	if not memo[filename] then 
		memo[filename] = true
		local lib = libs[filename]
		for u in pairs(lib.deps) do
			place(u)
		end 
		if not placed[filename] then
			placed[filename] = true
			table.insert(out, 1, filename)
		end
	end
end

for k, lib in pairs(libs) do
	place(k)
end


for i, v in ipairs(out) do
	print(i, v)
	for u in pairs(libs[v].deps) do
		--print(u, rank(u))
		if rank(u) < i then
			print("\t\twarning! rank(u) < i", u, rank(u))
		end
	end
end
