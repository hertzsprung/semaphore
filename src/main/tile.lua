--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

require 'compass'

local function get_track(track_list, direction)
	for i, track in ipairs(track_list) do
		local vector = Track.calculate_vector(track, direction)
		if vector then return vector end
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
		local is_occupied = self.occupier == train
		if is_occupied then
			logger:debug("Train '" .. train.name .. "' no longer occupies " .. tostring(self))
			self.occupier = nil
		else
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

	function Crossover:unoccupy(train)
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
				--self:set_train_properties(train)
				self.occupier = train
				return vector
			end
		elseif self.derail_protect and train.speed == TrainType.SLOW then
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
					--self:set_train_properties(train)
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
		-- TODO: need to decrement speed flag
	end

	-- set train speed to FAST if moving on to switched points
	-- FIXME: how to restore full speed after the whole train has moved off the points?
	function Junction:set_train_properties(train)
		if self.track:is_switched() and train.speed > TrainType.FAST then
			train.speed = TrainType.FAST
		end
	end

Flyover = {}

BufferStop = {}

