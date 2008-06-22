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
	for i = 1, math.floor(#blocks / 2), 1 do
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