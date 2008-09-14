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

TestPlatformTile = {}

	function TestPlatformTile:test_occupy()
		local actions = ActionList:new()
		local map = Map:new(5, 3)

		local station = Station:new("MYS", "My Station", Station.PASSENGER, {Train.COMMUTER})
		local platform = Platform:new(station, "1")
		local platform_tile1 = PlatformTile:new{
			vector=Vector:new(W, E),
			platform = platform,
			map = map
		}
		local platform_tile2 = PlatformTile:new{
			vector=Vector:new(W, E),
			platform = platform,
			map = map
		}
		local signal = Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.GREEN, vector=Vector:new(W, E)}

		local tile1 = map:set(1, 1, Track:new{vector=Vector:new(W, E)})
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new(W, E)})
		local tile3 = map:set(3, 1, platform_tile1)
		local tile4 = map:set(4, 1, platform_tile2)
		local tile5 = map:set(5, 1, signal)

		local train = Train:new{
			TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1),
			
			visible_head = 1,
			visible_tail = 2,
			map = map,
			actions = actions,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FAST,
			presence = Train.PRESENT,
			state = Train.MOVING
		}

		tile1.occupier = train; tile2.occupier = train;

		train:move(1, 1)
		assertEquals(train:speed(), TrainType.SLOW)
		assertEquals(tile1.occupier, nil)

		train:move(2, 2)
		assertEquals(tile2.occupier, nil)
	end
