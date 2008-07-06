--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

TrainType = {
	STOP = 0,
	SLOW = 1,
	FAST = 2,
	FULL = 3
}
	
	function TrainType:new(prefix, speeds)
		local o = {
			prefix = prefix,
			speeds = speeds
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

INTERCITY = TrainType:new('IC', {
	[TrainType.FULL] = 1,
	[TrainType.FAST] = 2,
	[TrainType.SLOW] = 3
})

COMMUTER = TrainType:new('LP', {
	[TrainType.FULL] = 2,
	[TrainType.FAST] = 3,
	[TrainType.SLOW] = 4
})

FREIGHT = TrainType:new('GD', {
	[TrainType.FULL] = 3,
	[TrainType.FAST] = 4,
	[TrainType.SLOW] = 5
})

TrainBlock = {}

	function TrainBlock.__tostring(o)
		return '[' .. o.position[1] .. ',' .. o.position[2] .. ' ' .. tostring(o.vector) .. ']'
	end

	function TrainBlock:new(position, vector, layer)
		local o = {
			position = position, -- the coordinate position on the map
			vector = vector,
			layer = layer -- the tile.layer 
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

Train = {
	STOPPED  = 1,
	MOVING   = 2,
	DERAILED = 3,
	CRASHED  = 4
}

function Train.__tostring(o)
	s = '{'
	for i, block in ipairs(o.blocks) do
		s = s .. tostring(block) .. ' '
	end
	s = s .. '} type=' .. tostring(o.type.prefix) .. ' speed=' .. ({'STOP', 'SLOW', 'FAST', 'FULL'})[o.speed + 1]
	return s
end

function Train:new(name, type, speed, state, blocks, length)
	length = length or #blocks
	local o = {
		name   = name,
		type   = type,
		speed  = speed,
		state  = state,
		length = length,
		blocks = blocks
	}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Train:head() return self.blocks[1] end

function Train:tail() return self.blocks[#self.blocks] end

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
	local head = self:head()
	local direction = head.vector[EXIT]
	local position = head.position:add(direction)

	logger:debug("Train '" .. self.name .. "' travelling " ..
		tostring(direction) .. " from " .. tostring(head.position)
		.. " to " .. tostring(position))

	local layer, route = self:route(direction, map:get(position))

	logger:debug("Train '" .. self.name .. "' routed " .. tostring(route))
	-- TODO: test for nil return

	self:tail().layer:set_occupier(nil)

	if layer.occupied then
		logger:warn("Train '" .. self.name .. "' has crashed: attempt to occupy the same tile layer as train " .. layer.occupied.name)
		-- TODO: crash
	else
		layer.occupied = self
	end
	local new_head = TrainBlock:new(position, route, layer)
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