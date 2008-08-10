--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('station')

TestStation = {}

	function TestStation:test_new()
		local station = Station:new("MYS", "My Station", Station.PASSENGER, {Train.INTERCITY, Train.COMMUTER})
		local p1 = Platform:new(station, "1")
		local p2 = Platform:new(station, "2")
		station.platforms = {p1, p2}
	end

TestPlatform = {}

	function TestPlatform:test_get_train_types()
		local station_train_types = {Train.INTERCITY, Train.COMMUTER}
		local station = Station:new("MYS", "My Station", Station.PASSENGER, station_train_types)
		local p1 = Platform:new(station, "1")

		local p2_train_types = {COMMUTER}
		local p2 = Platform:new(station, "2", p2_train_types)
		station.platforms = {p1, p2}
		assertEquals(p1:get_train_types(), station_train_types)
		assertEquals(p2:get_train_types(), p2_train_types)
	end
