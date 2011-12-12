local lpeg = require("lpeg")
local P, C, Cp, R, S, V = lpeg.P, lpeg.C, lpeg.Cp, lpeg.R, lpeg.S, lpeg.V

local co = coroutine
local floor = math.floor
local CodePad = require("app.CodePad")

module(..., package.seeall)

local newline_len = 0 --(CodePad.counts_newline==true) and 1 or 0

--[[
codepad:create_format{
	name = "default",
	color = {0, 0, 0},
}

codepad:create_format{
	name = "keyword",
	weight = 75,
	color = {0, 0, 0.8},
}

codepad:create_format{
	name = "comment",
	weight = 30,
	color = {0, 0.8, 0.1},
}

codepad:create_format{
	name = "number",
	weight = 40,
	color = {0, 0.5, 0.8},
}

codepad:create_format{
	name = "string",
	weight = 40,
	color = {0.8, 0.1, 0.1},
}

codepad:create_format{
	name = "start_long",
	weight = 40,
	color = {0.8, 0.1, 0.1},
}
--]]


----------------------------------------
--- Base patterns for text highlighting
-- digit pattern
local N = R'09'

-- alphanumeric pattern
local AZ = R('__','az','AZ','\127\255')

local SPACE = S'\n \t\r\f'

-- ensure a word ins't actually a sub-word
local END = -(N + AZ)

local symbols = {
	'+',     '-',     '*',     '/',     '%',     '^',     '#',
	'==',    '~=',    '<=',    '>=',    '<',     '>',     '=',
	'(',     ')',     '{',     '}',     '[',     ']',
	';',     ':',     ',',     '.',     '..',    '...',
}
local SYMBOLS = P(false)
for i, s in ipairs(symbols) do
	SYMBOLS = SYMBOLS + P(s)
end

local keywords = {
	'and',      'break',    'do',       'else',     'elseif',
	'end',      'false',    'for',      'function', 'if',
	'in',       'local',    'nil',      'not',      'or',
	'repeat',   'return',   'then',     'true',     'until',    'while',
}

local KEYWORD = P(false)
for i, word in ipairs(keywords) do
	KEYWORD = KEYWORD + P(word)*END
end

-- matches open tokens
local open = P"[" * P"="^0 * P"["

-- matches close tokens
local close = P"]" * P"="^0 * P"]"

-- letter followed by letters or numbers that isn't a keyword
local IDENTIFIER = AZ * (AZ+N)^0 - KEYWORD
local COMMENT = P"--" * (1 - P"\n")^0
local fraction = N^-1 * P"." * N + 
				 N * P"." +
				 N

local exponent = (P"e" + P"E") * S"+-"^-1 * N
local NUMBER = fraction * exponent^-1

local STRING = P'"' * ((1 - S("\"\\\n")))^0 * P'"'^-1 + 
				P"'" * ((1 - S('\'\\\n')))^0 * P"'"^-1
----------------------------------------

local
function capture_range(patt)
	return Cp()*patt*Cp()
end

local
function line_start(lines, line)
	local start = 0	
	for i=1, line-1 do
		start = start + lines[i].len
	end
	return start
end

local
function is_multiline_rule(rule)
	return	rule == "start_long" or 
			rule == "start_long_comment"
end

----------------------------------------
local C = {__index = _G}
local M = setmetatable(getfenv(), C)
M.__index = M


function C:__call(init)
	-- have to have a codepad to attach to
	assert(init.codepad, "no codepad provided")

	local m = setmetatable(init, M)
	
	init.codepad.lines = {}

	m:generate_almost_closed()
	m:generate_multiline_patterns()
	m:generate_highlight_patterns()
	m:attach(m.codepad)

	return m
end

function M:continue_line(line)
	local lines = self.codepad.lines
	local prev = lines[line-1]
	if(prev) then
		local current = lines[line]
		current.rule = prev.rule
		current.closing_token = prev.closing_token
	end
end

function M:set_multiline_state(line, name)
	local lines = self.codepad.lines
	lines[line].rule = name
	lines[line].closing_token = self.closing_token
end

function M:clear_multiline_state(line)
	local lines = self.codepad.lines
	lines[line].rule = nil
	lines[line].closing_token = nil
end

function M:in_multiline_mode()
	local lines = self.codepad.lines
	return	self.current_line > 1 and
			is_multiline_rule(lines[self.current_line-1].rule)
end

function M:generate_almost_closed()
	-- pattern for matching long close tokens 
	-- except for the one that matches the opening token
	local almost_closed = P{
		[1] = V"close",

		-- match-time check if close matches closing token
		close = #(close)*function(v, s, x)
			local match = v:sub(s)
			local e = close:match(match)
			match = match:sub(0, e-1)

			-- no match, push forward
			if(match ~= self.closing_token) then
				return s+e-1

			-- match closing token, fail
			else
				return nil
			end
		end,
	}

	self.almost_closed = (almost_closed + (1 - P"]"))^0
end

function M:generate_multiline_patterns()
	-- matches long string open until closing or end of string
	self.start_long = #open * function(subject, idx)
		-- extract opening braces
		local open = subject:match('^(%[=*%[)', idx)

		-- match up until closing braces
		-- extract token to match
		local closing_token = open:gsub('%[', '%]')
		if(not self.closing_token) then
			self.closing_token = closing_token
		end

		-- get the string after the opening token	
		local close_begin = idx+open:len()
		local remainder = subject:sub(close_begin)

		-- eat up until a match is found or the string ends
		local res = self.almost_closed:match(remainder)
		local nub = remainder:sub(res)

		-- get the potential ending token
		local token = nub:sub(0, self.closing_token:len())
	
		-- terminate early
		if(token == self.closing_token) then		
			return close_begin+res-1

		-- eat it all up
		else
			return subject:len()
		end
	end

	-- matches long string close token paired with an open token
	self.end_long = #close*function(subject, idx)
		if(self.closing_token) then
			local remainder = subject:sub(idx)
			local stop = close:match(remainder)
			local match = remainder:sub(0, stop-1)		

			if(match == self.closing_token) then	
				self.closing_token = nil
				return idx+stop-1
			end
		end
	end
	
	self.start_long_comment = P"--" * self.start_long
end


function M:generate_highlight_patterns()
	local rules = {
		[1] =	V"start_long_comment" + V"COMMENT" + V"KEYWORD" + V"STRING" + 
				V"start_long" + V"end_long" + V"NUMBER",

		-- modify rules to capture begin/end positions of match
		start_long_comment = capture_range(self.start_long_comment),
		COMMENT = capture_range(COMMENT*function(s, p)
			if(self:in_multiline_mode()) then
				return nil
			else
				return p
			end
		end),
		KEYWORD = capture_range(KEYWORD),
		STRING = capture_range(STRING),
		start_long = capture_range(self.start_long),
		end_long = capture_range(self.end_long),
		NUMBER = capture_range(NUMBER),
	}


	local
	function match_cb(name)
		return
		function(s, e)
			self.match(s, e, name)
		end
	end

	-- on match, callback into syntax highlighting syswtem
	local captures = {
		start_long_comment = match_cb("start_long_comment"),
		COMMENT = match_cb("comment"),
		KEYWORD = match_cb("keyword"),
		STRING = match_cb("string"),
		start_long = match_cb("start_long"),
		end_long = match_cb("end_long"),
		NUMBER = match_cb("number"),
	}

	-- apply captures to grammar
	for k, v in pairs(captures) do
		rules[k] = rules[k]/v
	end

	-- generate syntax highlighting pattern
	self.patt = (P(rules) + IDENTIFIER + SYMBOLS + SPACE)^0
end

function M:attach(codepad)
	-- coroutine handles per-line highlighting state
	-- and highlighting actions
	self.match = co.wrap(function()
		while(true) do
			local line, text = co.yield()
			if(text:len() > 0) then
				local lines = self.codepad.lines
			
				-- set highlighting for line to default
				local start = line_start(lines, line)
				codepad:format("default", start, start+text:len())

				-- handle the highlight callbacks
				local count = 0
				local multiline_mode = 	line > 1 and 
										is_multiline_rule(lines[line-1].rule)
				
				self.current_line = line
				local s, e, name = co.yield("match")
				while(s) do
					if(name == "end_long") then
						local highlight_start = start + ((count == 0) and 0 or s)
						local rule =	multiline_mode and lines[line-1].rule or 
										lines[line].rule	
						
						if(rule == "start_long_comment") then
							codepad:format("comment", highlight_start, start+e-1)
						else
							codepad:format("start_long", highlight_start, start+e-1)
						end
						
						codepad:format("default", start+e-1, start+text:len())
						self:clear_multiline_state(line)
						self.multiline_rule = nil
						count = count + 1
					elseif(name == "start_long") then
						local format
					
						if(multiline_mode and lines[line-1].rule ~= "start_long") then
							format = "comment"
							self:set_multiline_state(line, "start_long_comment")
						else
							format = name
							self:set_multiline_state(line, "start_long")
						end
					
						codepad:format(format, start+s-1, start+text:len())
						count = count + 1
					elseif(name == "start_long_comment") then
						local format
						if(multiline_mode and lines[line-1].rule ~= "start_long_comment") then
							format = "start_long"
							self:set_multiline_state(line, "start_long")
						else
							format = "comment"
							self:set_multiline_state(line, "start_long_comment")
						end
					
						codepad:format(format, start+s-1, start+text:len())
						count = count + 1
					elseif(not multiline_mode) then
						codepad:format(name, start+s-1, start+e-1)
						self:clear_multiline_state(line)
						count = count + 1
					end

					s, e, name = co.yield()
				end

				-- propagate previous line
				if(count == 0) then
					self:continue_line(line)

					-- should also apply highlighting
					if(lines[line].rule == "start_long") then
						codepad:format("start_long", start, start+text:len())
					elseif(lines[line].rule == "start_long_comment") then
						codepad:format("comment", start, start+text:len())
					else
						codepad:format("default", start, start+text:len())
					end
				end

			-- propagate state from previous line
			else
				self:continue_line(line)
			end
		end
	end)

	-- kick it off
	self.match()

	codepad.highlighter = self
	codepad.highlight = self:generate_highlight_cb()
	codepad.insert = self:generate_insert_cb()
	codepad.remove = self:generate_remove_cb()
end

function M:generate_insert_cb()
	return
	function (codepad, line, n)
		line = line + 1

		local text = ""
		for i=1, n do
			table.insert(codepad.lines, line+i, {
				text,
				len = text:len()+1,
			})
		end
	end
end

function M:generate_remove_cb()
	return
	function (codepad, line, n)
		line = line + 1

		for i=0, n+1, -1 do
			table.remove(codepad.lines, line)
		end
	end
end

function M:generate_highlight_cb()
	-- function highlights one or more lines depending 
	-- on the state of the line being modified
	-- if the current line creates or break a 
	-- multiline highlight, it will process 
	-- all affected lines
	return
	function(codepad, line, text)
		line = line+1

		-- restore state for previous line
		if(codepad.lines[line-1]) then
			if(is_multiline_rule(codepad.lines[line-1].rule)) then
				self.closing_token = codepad.lines[line-1].closing_token
			end
		end

		-- figure out how many lines to process
		local line_end = line
		local is_multiline = false
		if(codepad.lines[line]) then
			if(is_multiline_rule(codepad.lines[line].rule)) then
				is_multiline = true

				-- see how many lines could be affected by changes in this line
				while(codepad.lines[line_end]) do
					if(not is_multiline_rule(codepad.lines[line_end].rule)) then
						break
					end
					line_end = line_end + 1
				end
				if(not codepad.lines[line_end]) then
					line_end = line_end - 1
				end
			end
		end

		-- store the line
		codepad.lines[line] = {
			text,
			len = text:len()+newline_len,
		}

		-- highlight the lines
--		print("LINES: ", line, line_end)
		for i=line, line_end do
			local text = codepad.lines[i][1]
			local msg = self.match(i, text)
			if(msg == "match") then
				self.patt:match(text)
				self.match()
			end
		end

		-- if current line changed to a multiline, keep going until it stops
		if((not is_multiline) and is_multiline_rule(codepad.lines[line].rule)) then
			local next_line = line+1
			while(codepad.lines[next_line]) do
				local text = codepad.lines[next_line][1]
				local msg = self.match(next_line, text)
				if(msg == "match") then
					self.patt:match(text)
					self.match()
				end

				if(not is_multiline_rule(codepad.lines[next_line].rule)) then
					break
				end
				next_line = next_line+1
			end
		end
	end
end
