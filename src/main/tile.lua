--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'log'
local logger = log.logger

require 'compass'

local function get_track(track_list, direction)
	for key, track in pairs(track_list) do
		local vector = Track.calculate_vector(track, direction)
		if vector then return vector, key end
	end
end

BLANK = {}

	function BLANK:occupy(train)
		logger:error("train " .. tostring(train) .. " has crashed attempting to occupy blank tile")
		train:crash()
	end

Tile = {}
	function Tile:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Tile:unoccupy(train)
		local is_occupied = self:unoccupy_track(train)
		if not is_occupied then
			logger:error("Can't unoccupy " .. tostring(self) .. " because train '" .. train.name .. "' doesn't occupy it")
		end
		return is_occupied
	end

	function Tile:occupy_track(train, vector)
		if vector then
			if self.occupier then
				logger:info("train " .. tostring(train) .. " has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
			else
				self.occupier = train
				return vector
			end
		else
			logger:error("train " .. tostring(train) .. " has crashed because track wasn't connected")
			train:crash()
		end
	end

	function Tile:unoccupy_track(train)
		local is_occupied = self.occupier == train
		if is_occupied then
			logger:debug("Train '" .. train.name .. "' no longer occupies " .. tostring(self))
			self.occupier = nil
		end
		return is_occupied
	end

Track = Tile:new()

	function Track.__tostring(o)
		return '<Track ' .. tostring(o.vector) .. '>'
	end

	function Track:occupy(train)
		local vector = Track.calculate_vector(self.vector, train:direction())
		return self:occupy_track(train, vector)
	end

	function Track.calculate_vector(track_vector, train_direction)
		if track_vector[Vector.ENTRY] == train_direction.inverse then
			return track_vector
		end
		if track_vector[Vector.EXIT]  == train_direction.inverse then
			return track_vector:get_inverse()
		end
	end

Crossover = Tile:new()
	
	function Crossover.__tostring(o)
		local s = '<Crossover '
		for i, vector in ipairs(o.vectors) do
			s = s .. tostring(vector) .. ','
		end
		s = s .. '>'
		return s
	end

	function Crossover:occupy(train)
		local vector = get_track(self.vectors, train:direction())
		return self:occupy_track(train, vector)
	end

JunctionTrack = {}

	function JunctionTrack.__tostring(o)
		local s = "<JunctionTrack active={"
		for i, active in ipairs(o.active) do
			s = s .. tostring(active) .. " "
		end
		s = s .. "} inactive={"
		for i, inactive in ipairs(o.inactive) do
			s = s .. tostring(inactive) .. " "
		end
		s = s .. "}>"
		return s
	end

	function JunctionTrack:new(active, inactive)
		local o = {
			active = active,
			inactive = inactive,
			next = nil
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function JunctionTrack:is_switched()
		for i, v in ipairs(self.active) do
			if not v:is_straight() then
				return true
			end
		end
		return false
	end

Junction = {}

	function Junction:new(track, derail_protect)
		local o = {
			track = track,
			occupier = nil,
			derail_protect = derail_protect
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Junction:switch_points()
		if self.occupier then
			logger:warn("Can't switch points because tile is occupied by train" .. tostring(self.occupier))
			return false
		else
			self.track = self.track.next
			return true
		end
	end

	-- if track combinations allow, automatically switch points
	-- so that an inbound train can occupy this tile
	function Junction:auto_switch_points(direction)
		local state = self.track.next
		while state ~= self.track do
			local vector = get_track(state.active, direction)
			if vector then
				return state, vector
			end
			state = state.next
		end
		logger:debug("No suitable track state found for direction " .. tostring(direction))
	end

	function Junction:occupy(train)
		local vector = get_track(self.track.active, train:direction())
		if vector then
			if self.occupier then
				logger:info("train '" .. tostring(train) .. "' has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
			else
				self:set_train_properties(train, train.add_speed)
				self.occupier = train
				return vector
			end
		elseif self.derail_protect and train:speed() == TrainType.SLOW then
			if self.occupier then
				logger:info("train '" .. tostring(train) .. "' has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
			else
				logger:debug("Derail protection is enabled, finding suitable points combination for inbound train " .. tostring(train))
				local new_track, vector = self:auto_switch_points(train:direction())
				if new_track then
					logger:debug("auto-switching points to " .. tostring(new_track))
					self.track = new_track
					self:set_train_properties(train, train.add_speed)
					self.occupier = train
					return vector
				else
					logger:error("train " .. tostring(train) .. " has crashed because track wasn't connected")
					train:crash()
				end
			end
		else
			logger:debug("No active route and derail protection not applicable to inbound train " .. tostring(train))
			local inactive_vector = get_track(self.track.inactive, train:direction())
			if inactive_vector then
				train:derail()
			else
				logger:error("train " .. tostring(train) .. " has crashed because track wasn't connected")
				train:crash()
			end
		end
	end

	function Junction:unoccupy(train)
		local is_occupied = self:unoccupy_track(train)
		if not is_occupied then
			logger:error("Can't unoccupy " .. tostring(self) .. " because train '" .. train.name .. "' doesn't occupy it")
		else
			self:set_train_properties(train, train.remove_speed)
		end
		return is_occupied
	end

	-- add or remove train speed to FAST if moving on to switched points
	-- speed_function should be Train.add_speed() or Train.remove_speed()
	function Junction:set_train_properties(train, speed_function)
		if self.track:is_switched() and train:speed() > TrainType.FAST then
			speed_function(train, TrainType.FAST)
		end
	end

Flyover = {}

	function Flyover:new(layers)
		local o = {
			layers = layers -- a list of Tracks
		}
		setmetatable(o, self)
		self.__index = self
		o.vectors = {}
		for i, track in ipairs(o.layers) do
			o.vectors[track] = track.vector
		end
		return o
	end

	function Flyover:occupy(train)
		local vector, track = get_track(self.vectors, train:direction())
		if track then return track:occupy_track(train, vector) end
	end

	function Flyover:unoccupy(train)
		for i, layer in ipairs(self.layers) do
			if layer:unoccupy_track(train) then return true end
		end
		logger:error("Can't unoccupy " .. tostring(self) .. " because train '" .. train.name .. "' doesn't occupy it")
		return false
	end

BufferStop = {}

