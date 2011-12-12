local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable

local print = print

local math = require 'math'
local table = require 'table'

require 'muro'

local Matrix = muro.Matrix
local Op = muro.Op
local Magnitude = require 'matrixfx.magnitude'
local ThreeM = muro.ThreeM
local vec3 = muro.Vec3

local C = {}
local M = {
	outmat,	-- output matrix (adapts to input)
	tmpmat,	-- temp matrix for magnitude scaling calculations
	mag,	-- magnitude filter
	threem,	-- min/mean/max operator
	add,	-- addition matrix operator
	mul,	-- multiply matrix operator
	log,	-- log matrix operator
	div,	-- division matrix operator
}

local params = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end


	self.add:calc(magmat, tmpmat)			-- mag+1
	self.log:calc(tmpmat, tmpmat)			-- log(mag+1)
	self.mul.vals = inv_logbase
	self.mul:calc(tmpmat, tmpmat)			-- normmag = log(mag+1)/log(base)
--	self.max:scale(tmpmat, tmpmat)			-- normmag = max(normmag, eps)
	self.div:calc({tmpmat, mag}, tempmat)	-- scale = normmag/mag
	self.mul:calc({inmat, tmpmat}, outmat)	-- grad*scale

module('matrixfx.lognormalize', setconstructor)

--[[------------------------------------------------------
	Shapes the magnitudes of a vectorfield into a reasonable dynamic range, 
	[0, 1] for  visualization and simulation purposes while preserving its 
	features such as zero magnitudes and sign.  The equation used is:
	
		local mag = vec3.mag(v)
		local s = log(mag+1)/log(base)
		v = v*s/mag
	
	where base is the maximum magnitude + 1
		and	
	mag > 0.

	if mag is 0., then it is passed through
--]]

----------------------------------------------------------
-- @param

function C:__call()
	-- setup fx as an extension of Texture
	local m = {}
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.outmat = Matrix()
	m.tmpmat = Matrix()
	m.mag = Magnitude()
	m.threem = ThreeM()
	
	m.add = Op{
		op = Op.ADD,
		vals = {1},
	}
	
	m.mul = Op{
		op = Op.MUL,
	}
	
	m.log = Op{
		op = Op.LOG,
		vals = {2},	-- default
	}
	
	m.div = Op{
		op = Op.DIV,
	}

	
	return m
end

local math_log = math.log
function M:calc(inmat)
	local outmat = self.outmat
	outmat:adapt(inmat)
	
	-- get the magnitude of the incoming matrix
	local magmat = self.mag:calc(inmat)	
	
	-- calculate the log base to lognorm with
	local threem = self.threem
	threem:calc(magmat)
	local base = threem.max[1]+1
	local inv_logbase = 1./math_log(base)
	
	-- lognorm the magnitudes
	local tmpmat = self.tmpmat
	tmpmat:adapt(magmat)
	self.add:calc(magmat, tmpmat)			-- mag+1
	self.log:calc(tmpmat, tmpmat)			-- log(mag+1)
	self.mul.vals = inv_logbase
	self.mul:calc(tmpmat, tmpmat)			-- normmag = log(mag+1)/log(base)
--	self.max:scale(tmpmat, tmpmat)			-- normmag = max(normmag, eps)
	self.div:calc({tmpmat, mag}, tempmat)	-- scale = normmag/mag
	self.mul:calc({inmat, tmpmat}, outmat)	-- grad*scale
	
	return outmat
end

--[[
print("************************")
print("min", table.concat(self.threem.min, " "))
print("mean", table.concat(self.threem.mean, " "))
print("max", table.concat(self.threem.max, " "))
--]]