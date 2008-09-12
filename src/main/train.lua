--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'log'
local logger = log.logger

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
	CRASHED  = 4,

	ABSENT   = 1,
	ENTERING = 2,
	PRESENT  = 3,
	EXITING  = 4
}

	function Train.__tostring(o)
		s = '{'
		for i, block in ipairs(o) do
			s = s .. tostring(block) .. ' '
		end
		s = s .. '} type=' .. tostring(o.type.prefix)
		s = s .. ' speed=' .. ({'STOP', 'SLOW', 'FAST', 'FULL'})[o:speed() + 1]
		return s
	end
	
	--[[
		Accepts a table containing
		1..n:TrainBlock 1 is the head of the train, n the tail
		map:Map  -- TODO: might want to just have Game instead of Map and ActionList
		actions:ActionList
		name:String
		type:TrainType
		signal_speed:TrainType.FULL/FAST/SLOW/STOP
		presence:Train.ABSENT/ENTERING/PRESENT/EXITING
		state:Train.STOPPED/MOVING/DERAILED/CRASHED
		visible_head:int (optional)
		visible_tail:int (optional)
	]]--
	function Train:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		o.speeds = {}
		-- TODO: should these fields be optional?
		o.visible_head = o.visible_head or 1
		o.visible_tail = o.visible_tail or 0
		return o
	end
	
	function Train:head() return self[self.visible_head] end
	
	function Train:tail() return self[#self] end
	
	function Train:reverse()
		for i = 1, math.ceil(#self / 2) do
			local first = self[i]
			local last  = self[#self - i + 1]
			first.vector:inverse()
			if first ~= last then
				last.vector:inverse()
				self[i] = last
				self[#self - i + 1] = first
			end
		end
	end

	function Train:shift(head)
		for i = self.visible_tail, self.visible_head + 1, -1 do
			self[i] = self[i-1]
		end
		self[self.visible_head] = head
	end
	
	function Train:direction()
		return self:head().vector[Vector.EXIT]
	end

	function Train:move(requested_time, actual_time)
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
		
			self:update_entering_state()
			if self.presence == Train.EXITING or self.presence == Train.PRESENT then
				self:tail().tile:unoccupy(self)
			end
			local new_head = TrainBlock:new(position, new_direction, tile)
			self:shift(new_head)
			self:update_exiting_state()

			local next_move = requested_time + self.type.speeds[self:speed()]
			logger:debug("Train '" .. self.name .. "' next moving at " .. next_move)

			local move_action = function (actions, requested_time, actual_time)
				self:move(requested_time, actual_time)
			end
			self.actions:add(move_action, next_move)
		else
			logger:debug("Train '" .. self.name .. "' stopped moving")
			if self.state == Train.MOVING then self.state = Train.STOPPED end
		end
	end

	-- private
	function Train:update_entering_state()
		if self.presence == Train.ENTERING then
			self.visible_tail = self.visible_tail + 1
			if self.visible_tail == #self then
				logger:debug("Train '" .. self.name .. "' has completed ENTERING, becoming PRESENT")
				self.presence = Train.PRESENT
			end
		end
	end

	--private
	function Train:update_exiting_state()
		if self.presence == Train.EXITING then
			self.visible_head = self.visible_head + 1
			if self.visible_head > #self then
				logger:debug("Train '" .. self.name .. "' has completed EXITING, becoming ABSENT")
				self.presence = Train.ABSENT
			end
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

	function Train:unblock_signal(signal)
		-- TODO: won't want to unblock when you're sat in a station, for example
		logger:debug("Train " .. tostring(train) .. " has been unblocked by signal " .. tostring(self))
		self.state = Train.MOVING
		self.signal_speed = TrainType.FULL

		local move_action = function (actions, requested_time, actual_time)
			self:move(requested_time, actual_time)
		end
		self.actions:add(move_action)
	end