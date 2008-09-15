--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'log'
local logger = log.logger

require('train')
require('compass')
require('tile')

-- made this a class in its own right so that scheduling can be attached to StationTypes
StationType = {}

	function StationType:new()
		local o = {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

Station = {
	PASSENGER = StationType:new(),
	-- a train scheduled to occupy a siding platform will, upon leaving,
	-- be scheduled to occupy a passenger station platform
	SIDING = StationType:new(),
	DEPOT = StationType:new()
}

	function Station.__tostring(o)
		return '<Station ' .. tostring(o.code) .. '>'
	end
	
	function Station:new(code, name, type, train_types)
		local o = {
			code = code,
			name = name,
			-- PASSENGER, SIDING or DEPOT
			type = type,
			-- a list of TrainTypes that can be scheduled to occupy the station
			train_types = train_types,
			platforms = {}
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Station:add_platform(platform)
		table.insert(self.platforms, platform)
	end

	function Station:bounding_box()
		local platform_boxes = {}
		for i, platform in ipairs(self.platforms) do
			local min, max = platform.bounding_box()
			table.insert(platform_boxes, min)
			table.insert(platform_boxes, max)
		end
		return Coord.bounding_box(platform_boxes)
	end

Platform = {}

	function Platform.__tostring(o)
		return '<Platform ' .. tostring(o.name) .. ' belongs-to ' .. tostring(o.station) .. '>'
	end

	function Platform:new(station, name, train_types, tiles)
		local o = {
			-- the station to which the platform belongs
			station = station,
			name = name,
			-- train_types is an optional override to allow a platform
			-- to be occupied by a different set of train types to its station
			train_types = train_types,
			-- the train(s) currently occupying the platform
			-- wouldn't expect more than one train, though it is conceivable
			-- without them crashing immediately
			occupation = {},
			tiles = tiles -- list of Coords, not Tiles
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Platform:get_train_types()
		return self.train_types or self.station.train_types
	end

	function Platform:bounding_box()
		return Coord.bounding_box(self.tiles)
	end

-- in addition to properties required by Track
-- platform:Platform
-- map:Map
-- actions:ActionList
-- TODO: this class is currently implementing a suburban station
PlatformTile = Track:new()

	function PlatformTile.__tostring(o)
		return '<PlatformTile ' .. tostring(o.vector) .. ' ' .. tostring(o.platform) .. '>'
	end

	function PlatformTile:occupy(train, position, requested_time)
		local occupy = Track.occupy(self, train)
		if occupy then
			train:add_speed(TrainType.SLOW)
			local next_tile = self.map:get(position:add(occupy[Vector.EXIT]))
			if getmetatable(next_tile) ~= PlatformTile then
				logger:debug('Next tile after this PlatformTile is a non-PlatformTile ' .. tostring(next_tile))
				train:stop()
				local next_move = requested_time + 10000 -- FIXME: hardwired dwell time
	
				local move_action = function (actions, requested_time, actual_time)
					train:resume()
					train:move(requested_time, actual_time)
				end
				self.actions:add(move_action, next_move)
			end
		end
		return occupy
	end

	function PlatformTile:unoccupy(train)
		local occupy = Track.unoccupy(self, train)
		if occupy then
			train:remove_speed(TrainType.SLOW)
		end
		return occupy
	end

Spawn = {}

	function Spawn:new(code, name, entry_tile, exit_tile, entry_types, exit_types)
		local o = {
			code = code,
			name = name,
			entry_tile = entry_tile, -- Coords, not Tiles
			exit_tile = exit_tile,
			entry_types = entry_types, -- list of TrainTypes
			exit_types = exit_types
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Spawn:bounding_box()
		return Coord.bounding_box({self.entry_tile, self.exit_tile})
	end

SpawnEntry = {}

SpawnExit = {}

	function SpawnExit.__tostring(o)
		return '<SpawnExit ' .. tostring(o.compass) .. '>'
	end

	--[[
		compass:Compass the direction from which the exit tile accepts trains
		spawn:Spawn
	]]--
	function SpawnExit:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function SpawnExit:occupy(train)
		if self.compass == train:direction().inverse then
			if train.presence ~= Train.EXITING then
				train.presence = Train.EXITING
			end
			return Vector:new(self.compass,CENTRE)
		else
			logger:error("train " .. tostring(train) .. " has crashed because track wasn't connected")
			train:crash()
		end
	end
