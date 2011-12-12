local debug = require("debug")

module(..., package.seeall)
----------------------------------------
local C = {__index = _G}
local M = setmetatable(getfenv(), C)
M.__index = M

function C:__call(init)
	local m = setmetatable(init or {}, M)
	
	m.chunks = {}
	
	return m
end

local
function print_table(t, name, lvl)
	lvl = lvl or 0
	print(string.rep("\t", lvl) .. (name and (name .. " = {") or "{"))
	for k, v in pairs(t) do
		if(type(v) == "table") then
			print_table(v, k, lvl+1)
		else
			print(string.rep("\t", lvl+1) .. tostring(k).."\t".."'"..tostring(v).."'")
		end
	end
	print(string.rep("\t", lvl) .. "}")
end

function M:register_chunk(name, code)
	self.chunks[name] = code
end

function M:debug(codepad, f)
	local current_line
	local current_chunk
	local enable_debugging = false
	local stack_depth = 0
	local chunknames = {}
	
	
	local
	function hook(event, line)
		if(not enable_debugging) then
			return
		end

		local current_info = debug.getinfo(2, "nSlufL") or {}
		if(event == "call") then
			print_table(current_info)
			chunknames[#chunknames+1] = current_info.source
		elseif(event == "line") then
			--print(debug.traceback())
			local level = 2
			while(true) do
				local dbg_info = debug.getinfo(level, "nSlufL")
				if(not dbg_info) then
					break
				end
				if(dbg_info.currentline < 0) then
					break
				end
				print("stack: ", level, dbg_info.source, dbg_info.currentline)
				level = level + 1
			end
			
			current_line = line
			current_chunk = chunknames[#chunknames]
	--		print(chunknames[#chunknames], event, line)
			
			prev_line = line
		elseif(event == "return") then
		--	print_table(current_info)
			print(chunknames[#chunknames], event, current_info.source)
			print(table.concat(chunknames, ", "))

			if(chunknames[#chunknames] ~= current_info.source) then
				print("ERROR: ", current_line, current_chunk)
				-- pause execution of debuffer
				enable_debugging = false -- should call debug.debug()
				
				local chunk = self.chunks[ chunknames[#chunknames] ]
				
				local l = 1
				local lines = {}
				for line in chunk:gmatch("([^\n]*\n?)") do
					lines[l] = line
					
					if(current_line == l) then
						print("ERROR LINE: ", line)
					end
					l = l+1
				end
				
				--[[
				for i, line in ipairs(codepad.lines) do
					if(line[1] == lines[1]) then
						local j = 2
						while(j <= #lines and 
							i+j-1 <= #codepad.lines and
							codepad.lines[i+j-1][1]:find(lines[j], nil, true))
						do
							j = j+1
						end
						
						if(j > #lines) then
							print("ERROR X LINE: ", i+current_line-1, 
										codepad.lines[i+current_line-1][1])
							break
						end
					end
				end
				--]]
			end
			
			chunknames[#chunknames] = nil
		end
		print("*************************")
	end
	
	local ok, res = pcall(function()
		debug.sethook(hook, "crl")
		enable_debugging = true
		f()
		enable_debugging = false
		debug.sethook(nil)
	end)
--	print(ok, res)
end