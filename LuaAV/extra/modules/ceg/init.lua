local m = require("lpeg")
local table = require("table")

local print = print
local ipairs = ipairs
local pairs = pairs
local getfenv = getfenv

local P, V, R, S = m.P, m.V, m.R, m.S

local c99 = require("ceg.c99")

module("ceg")
-------------------------------------------

local EOF = P(-1)

function combine(t1, t2, ...)
	if(t2) then
		local t = {}
		for k, v in pairs(t2) do
			t[k] = v
		end
		for k, v in pairs(t1) do
			t[k] = v
		end
		return combine(t, ...)
	else
		return t1
	end
end

function apply(g, c)
	for k, cc in pairs(c) do
		g[k] = g[k]/cc
	end
	return P(g)
end

function scan(patt)
	return (patt + c99.any)^0 * EOF
end