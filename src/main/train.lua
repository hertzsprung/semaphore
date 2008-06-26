--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

STOPPED = 1
MOVING  = 2
CRASHED = 3

TrainBlock = {}

function TrainBlock:new(position, vector)
	local o = {
		position = position,
		vector = vector
	}
	setmetatable(o, self)
	self.__index = self
	return o
end

Train = {}

function Train.__tostring(o)
	s = '{'
	for i, block in ipairs(o.blocks) do
		s = s .. '[' .. block.position[1] .. ',' .. block.position[2]
		s = s .. ' ' .. tostring(block.vector) .. '] '
	end
	s = s .. '} '
	return s
end

function Train:new(state, blocks, length)
	length = length or #blocks
	local o = {
		state  = state,
		length = length,
		blocks = blocks
	}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Train:reverse()
	local blocks = self.blocks
	for i = 1, math.floor(#blocks / 2) do
		first = blocks[i]
		last  = blocks[#blocks - i + 1]
		first.vector:inverse()
		last.vector:inverse()
		blocks[i] = last
		blocks[#blocks - i + 1] = first
	end
end

function Train:shift(head)
	local blocks = self.blocks
	for i = #blocks, 2, -1 do
		blocks[i] = blocks[i-1]
	end
	blocks[1] = head
end

function Train:move(map)
	local head = self.blocks[1]
	local direction = head.vector[EXIT]
	local position = head.position:add(direction)
	local layer, route = self:route(direction, map:get(position))
	-- TODO: decrement ref count on the tail's map tile
	-- TODO: test for nil return
	local new_head = TrainBlock:new(position, route)
	self:shift(new_head)
end	

function Train:route(direction, tile)
	for i, layer in ipairs(tile.layers) do
		for j, route in ipairs(layer.active) do
			if route[ENTRY] == direction.inverse then
				return layer, route
			end
			if route[EXIT]  == direction.inverse then
				return layer, Vector:new{route[EXIT], route[ENTRY]}
			end
		end
	end
end