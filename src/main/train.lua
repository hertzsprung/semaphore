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

TrainBlock = {}

	function TrainBlock.__tostring(o)
		return '[' .. o.position.x .. ',' .. o.position.y .. ' ' .. tostring(o.vector) .. ']'
	end

	function TrainBlock:new(position, vector, tile)
		local o = {
			position = position, -- the coordinate position on the map
			vector = vector,
			tile = tile
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

Train = {
	INTERCITY = TrainType:new('IC', {
		[TrainType.FULL] = 1,
		[TrainType.FAST] = 2,
		[TrainType.SLOW] = 3
	}),
	
	COMMUTER = TrainType:new('LP', {
		[TrainType.FULL] = 2,
		[TrainType.FAST] = 3,
		[TrainType.SLOW] = 4
	}),
	
	FREIGHT = TrainType:new('GD', {
		[TrainType.FULL] = 3,
		[TrainType.FAST] = 4,
		[TrainType.SLOW] = 5
	}),

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
	
	function Train:direction()
		return self:head().vector[Vector.EXIT]
	end

	function Train:move(map)
		local head = self:head()
		local direction = self:direction()
		local position = head.position:add(direction)
	
		logger:debug("Train '" .. self.name .. "' travelling " ..
			tostring(direction) .. " from " .. tostring(head.position)
			.. " to " .. tostring(position))
	
		local tile = map:get(position)
		local new_direction = tile:occupy(self, direction)
		-- TODO: test tile return
		logger:debug("Train '" .. self.name .. "' routed to " .. tostring(tile) .. " new direction " .. tostring(new_direction))
	
		self:tail().tile:unoccupy(self)
		local new_head = TrainBlock:new(position, new_direction, tile)
		self:shift(new_head)
	end	

	function Train:crash()
		self.speed = TrainType.STOP
		self.state = Train.CRASHED
	end