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

BLANK = {}

Track = {}

	function Track.__tostring(o)
		return tostring(o.vector) .. ' Track'
	end
	
	function Track:new(vector)
		local o = {
			vector = vector,
			occupier = nil -- train occupying the tile
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Track:occupy(train, direction)
		local vector = Track.calculate_vector(self.vector, direction)
		if vector then
			if self.occupier then
				logger:info("train " .. tostring(train) .. " has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
				return nil
			else
				self.occupier = train
			end
		end
		return vector
	end

	function Track:unoccupy(train)
		local is_occupied = self.occupier == train
		if is_occupied then
			logger:debug("Train '" .. train.name .. "' no longer occupies " .. tostring(self))
			self.occupier = nil
		else
			logger:error("Can't unoccupy " .. tostring(self) .. " because train '" .. train.name .. "' doesn't occupy it")
		end
		return is_occupied
	end

	function Track.calculate_vector(track_vector, train_direction)
		if track_vector[Vector.ENTRY] == train_direction.inverse then
			return track_vector
		end
		if track_vector[Vector.EXIT]  == train_direction.inverse then
			return track_vector:get_inverse()
		end
	end

JunctionTrack = {}

	function JunctionTrack:new(active, ...)
		local o = {
			active = active,
			inactive = {},
			next = nil
		}
		for i, inactive in ipairs(...) do
			table.insert(o.inactive, inactive)
		end
		setmetatable(o, self)
		self.__index = self
		return o
	end

Junction = {}

	function Junction:new(track)
		local o = {
			track = track,
			occupier = nil
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Junction:switch_points()
		if self.occupier then
			logger:warn("Can't switch points because tile is occupied train" .. tostring(self.occupier))
			return false
		else
			self.track = self.track.next
			return true
		end
	end

Flyover = {}

BufferStop = {}

