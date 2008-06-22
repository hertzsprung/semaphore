--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

local Compass = {}

function Compass.__eq(o1, o2)
	return o1[1] == o2[1] and o1[2] == o2[2]
end

function Compass.__tostring(o)
	return o.name
end

function Compass:new(x, y, name)
	o = {x, y, name=name}
	setmetatable(o, self)
	self.__index = self
	return o
end

N  = Compass:new( 0, 1, 'N')
NE = Compass:new( 1, 1, 'NE')
E  = Compass:new( 1, 0, 'E')
SE = Compass:new( 1,-1, 'SE')
S  = Compass:new( 0,-1, 'S')
SW = Compass:new(-1,-1, 'SW')
W  = Compass:new(-1, 0, 'W')
NW = Compass:new(-1, 1, 'NW')

N.inverse  = S
NE.inverse = SW
E.inverse  = W
SE.inverse = NW
S.inverse  = N
SW.inverse = NE
W.inverse  = E
NW.inverse = SE

Vector = {}

function Vector.__eq(o1, o2)
	return o1[1] == o2[1] and o1[2] == o2[2]
end

function Vector.__tostring(o)
	return tostring(o[1]) .. '-' .. tostring(o[2])
end

function Vector:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Vector:inverse()
	local tmp = self[2]
	self[2] = self[1]
	self[1] = tmp
	return self
end