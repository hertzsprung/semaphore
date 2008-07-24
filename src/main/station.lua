--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('train')

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
		self.platforms[#self.platforms+1] = platform
	end

Platform = {}

	function Platform:new(station, name, train_types)
		local o = {
			-- the station to which the platform belongs
			station = station,
			name = name,
			-- train_types is an optional override to allow a platform
			-- to be occupied by a different set of train types to its station
			train_types = train_types,
			-- the train currently occupying the platform
			occupier = nil
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Platform:get_train_types()
		return self.train_types or self.station.train_types
	end