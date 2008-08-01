--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('tile')
require('train')

TestTrack = {}

	function TestTrack.test_calculate_vector()
		local track_vector = Vector:new{W, NE}
		local vector = Track.calculate_vector(track_vector, E)
		assertEquals(vector, track_vector)

		vector = Track.calculate_vector(track_vector, SW)
		assertEquals(vector, Vector:new{NE, W})

		vector = Track.calculate_vector(track_vector, N)
		assertEquals(vector, nil)
	end

	function TestTrack.test_occupy()
		local track = Track:new(Vector:new{W, NE})
		local train = Train:new("mytrain", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, track) }, 1)

		local vector = track:occupy(train, train:direction())
		assertEquals(vector, Vector:new{W, NE})
	end

	function TestTrack.test_occupy_bad_track()
		local train = Train:new("mytrain", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{S, N}, track) }, 1)

		local track = Track:new(Vector:new{W, NE})
		local vector = track:occupy(train, train:direction())
		assertEquals(vector, nil)
		assertEquals(train.state, Train.CRASHED)
		assertEquals(train.speed, TrainType.STOP)
	end

	function TestTrack.test_occupy_crash()
		local train1 = Train:new("mytrain1", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, track) }, 1)
		local train2 = Train:new("mytrain2", Train.COMMUTER, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, track) }, 1)

		local track = Track:new(Vector:new{W, NE})
		track:occupy(train1, train1:direction())
		local vector = track:occupy(train2, train2:direction())
		assertEquals(vector, nil)
		assertEquals(train1.state, Train.CRASHED)
		assertEquals(train1.speed, TrainType.STOP)
		assertEquals(train2.state, Train.CRASHED)
		assertEquals(train2.speed, TrainType.STOP)
	end

	function TestTrack.test_unoccupy()
		local track = Track:new(Vector:new{W, NE})

		local train1 = Train:new("mytrain1", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, track) }, 1)

		local train2 = Train:new("mytrain2", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(2, 2), Vector:new{W, E}, track) }, 1)

		track:occupy(train1, train1:direction())

		assertEquals(track:unoccupy(train1), true)
		assertEquals(track:unoccupy(train2), false)
	end

TestJunctionTrack = {}

	function TestJunctionTrack.test_is_switched()
		local junction_track = JunctionTrack:new({Vector:new{W, NE}}, {Vector:new{W, E}})
		assertEquals(junction_track:is_switched(), true)

		junction_track = JunctionTrack:new({Vector:new{W, E}}, {Vector:new{W, NE}})
		assertEquals(junction_track:is_switched(), false)
	end

TestJunction = {}

	function TestJunction.test_switch_pointsd()
		local track1 = JunctionTrack:new({Vector:new{W, NE}}, {Vector:new{W, E}})
		local track2 = JunctionTrack:new({Vector:new{W, E}}, {Vector:new{W, NE}})
		track1.next = track2
		track2.next = track1

		local junction = Junction:new(track1)
		local success = junction:switch_points()
		assertEquals(success, true)
		assertEquals(junction.track, track2)

		local train = Train:new("mytrain", Train.INTERCITY, TrainType.FULL, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{SE, W}, track) }, 1)
		junction:occupy(train)
		success = junction:switch_points()
		assertEquals(success, false)
		assertEquals(junction.track, track2)
	end

LuaUnit:run()