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
		local train1 = Train:new("mytrain1", Train.INTERCITY, TrainType.FAST, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, track) }, 1)

		local vector = track:occupy(train1, train1:direction())
		assertEquals(vector, Vector:new{W, NE})
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

LuaUnit:run()