--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

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

	function Platform:new(station, name, train_types, tiles)
		local o = {
			-- the station to which the platform belongs
			station = station,
			name = name,
			-- train_types is an optional override to allow a platform
			-- to be occupied by a different set of train types to its station
			train_types = train_types,
			-- the train currently occupying the platform
			occupier = nil,
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
