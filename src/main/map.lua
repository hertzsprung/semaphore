--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('tile')

Map = {}

function Map.__tostring(o)
	s = ''
	for i = 1, o.w do
		for j = o.h, 1, -1 do
			s = s .. tostring(o[j][i]) .. ' '
		end
		s = s .. '\n'
	end
	return s
end

function Map:new(w, h)
	local o = {
		w = w,
		h = h,
	}

	for y = 1, h do
		o[y] = {}
		for x = 1, w do
			o[y][x] = TILES[0]
		end
	end

	setmetatable(o, self)
	self.__index = self
	return o
end

function Map:set(x, y, tile)
	if tonumber(x) == nil or x < 1 or x > self.w then
		error(x .. ' invalid for map of size ' .. self.w .. 'x' .. self.h)
	end
	if tonumber(y) == nil or y < 1 or y > self.h then
		error(y .. ' invalid for map of size ' .. self.h .. 'x' .. self.h)
	end
	self[y][x] = tile
end
