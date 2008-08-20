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

	function TrainBlock.__eq(o1, o2)
		return o1.position == o2.position and o1.vector == o2.vector and o1.tile == o2.tile and o1.visible == o2.visible
	end

	function TrainBlock:new(position, vector, tile)
		local o = {
			position = position, -- the coordinate position on the map
			vector = vector,
			tile = tile,
			visible = true
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
		s = s .. '} type=' .. tostring(o.type.prefix)
		s = s .. ' speed=' .. ({'STOP', 'SLOW', 'FAST', 'FULL'})[o:speed() + 1]
		return s
	end
	
	function Train:new(map, name, type, speed, state, blocks, length)
		length = length or #blocks
		local o = {
			map    = map,
			name   = name,
			type   = type,
			signal_speed = speed,
			speeds = {},
			state  = state,
			length = length,
			blocks = blocks
		}
		setmetatable(o, self)
		self.__index = self
		o.signal_speed = speed
		return o
	end
	
	function Train:head() return self.blocks[1] end
	
	function Train:tail() return self.blocks[#self.blocks] end
	
	function Train:reverse()
		local blocks = self.blocks
		for i = 1, math.ceil(#blocks / 2) do
			local first = blocks[i]
			local last  = blocks[#blocks - i + 1]
			first.vector:inverse()
			if first ~= last then
				last.vector:inverse()
				blocks[i] = last
				blocks[#blocks - i + 1] = first
			end
		end
	end

	function Train:shift(head)
		local blocks = self.blocks
		local shifts
		if #blocks < self.length then
			shifts = #blocks + 1
		else
			shifts = #blocks
		end
		for i = shifts, 2, -1 do
			blocks[i] = blocks[i-1]
		end
		blocks[1] = head
	end
	
	function Train:direction()
		return self:head().vector[Vector.EXIT]
	end

	function Train:move(actions, requested_time, actual_time)
		local head = self:head()
		local direction = self:direction()
		local position = head.position:add(direction)
	
		logger:debug("Train '" .. self.name .. "' travelling " ..
			tostring(direction) .. " from " .. tostring(head.position)
			.. " to " .. tostring(position))
	
		local tile = self.map:get(position)
		local new_direction = tile:occupy(self, direction)
		if new_direction then
			logger:debug("Train '" .. self.name .. "' routed to " .. tostring(tile) .. " new direction " .. tostring(new_direction))
		
			if #self.blocks == length then self:tail().tile:unoccupy(self) end
			local new_head = TrainBlock:new(position, new_direction, tile)
			self:shift(new_head)

			local next_move = requested_time + self.type.speeds[self:speed()]
			logger:debug("Train '" .. self.name .. "' next moving at " .. next_move)

			local move_action = function (actions, requested_time, actual_time)
				self.move(self, actions, requested_time, actual_time)
			end
			actions:add(next_move,move_action)
		else
			logger:debug("Train '" .. self.name .. "' stopped moving")
			if self.state == Train.MOVING then self.state = Train.STOPPED end
		end
	end

	function Train:speed()
		for i, s in ipairs({TrainType.STOP, TrainType.SLOW, TrainType.FAST, TrainType.FULL}) do
			if self.signal_speed == s or (self.speeds[s] and self.speeds[s] ~= 0) then return s end
		end
		return TrainType.STOP
	end

	function Train:add_speed(s)
		self.speeds[s] = (self.speeds[s] or 0) + 1
	end

	function Train:remove_speed(s)
		self.speeds[s] = (self.speeds[s] or 1) - 1
	end

	function Train:crash()
		self.speed = self:add_speed(TrainType.STOP)
		self.state = Train.CRASHED
	end

	function Train:derail()
		self.speed = self:add_speed(TrainType.STOP)
		self.state = Train.DERAILED
	end