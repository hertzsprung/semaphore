--[[
This file is part of librosprite.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

Compass = {}

function Compass.__eq(o1, o2)
	return o1[1] == o2[1] and o1[2] == o2[2]
end

function Compass:inverse()
	return Compass:new{self[1]*-1, self[2]*-1}
end

function Compass:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

N  = Compass:new{ 0, 1}
NE = Compass:new{ 1, 1}
E  = Compass:new{ 1, 0}
SE = Compass:new{ 1,-1}
S  = Compass:new{ 0,-1}
SW = Compass:new{-1,-1}
W  = Compass:new{-1, 0}
NW = Compass:new{-1, 1}