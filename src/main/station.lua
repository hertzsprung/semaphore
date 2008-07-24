--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('train')

Platform = {}

	function Platform:new(name, train_types)
		local o = {
			name = name,
			-- train_types is an optional override to allow a platform
			-- to be occupied by a different set of train types to its station
			train_types = train_types
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

Station = {
	PASSENGER = 1,
	-- a train scheduled to occupy a siding platform will, upon leaving,
	-- be scheduled to occupy a passenger station platform
	SIDING = 2,
	DEPOT = 3
}
	
	function Station:new(code, name, type, train_types, platforms)
		local o = {
			code = code,
			name = name,
			-- PASSENGER, SIDING or DEPOT
			type = type,
			-- a list of TrainTypes that can be scheduled to occupy the station
			train_types = train_types,
			platforms = platforms
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end