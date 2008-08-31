--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('tile')
require('train')

TestBlank = {}

	function TestBlank.test_occupy()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)
		local vector = BLANK:occupy(train)
		assertEquals(vector, nil)
		assertEquals(train.state, Train.CRASHED)
	end

TestTrack = {}

	function TestTrack.test_calculate_vector()
		local track_vector = Vector:new(W, NE)
		local vector = Track.calculate_vector(track_vector, E)
		assertEquals(vector, track_vector)

		vector = Track.calculate_vector(track_vector, SW)
		assertEquals(vector, Vector:new(NE, W))

		vector = Track.calculate_vector(track_vector, N)
		assertEquals(vector, nil)
	end

	function TestTrack.test_occupy()
		local track = Track:new{vector=Vector:new(W, NE)}
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)

		local vector = track:occupy(train, train:direction())
		assertEquals(vector, Vector:new(W, NE))
	end

	function TestTrack.test_occupy_bad_track()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(S, N), track) }, 1)

		local track = Track:new{vector=Vector:new(W, NE)}
		local vector = track:occupy(train, train:direction())
		assertEquals(vector, nil)
		assertEquals(train.state, Train.CRASHED)
		assertEquals(train:speed(), TrainType.STOP)
	end

	function TestTrack.test_occupy_crash()
		local train1 = Train:new(nil, "mytrain1", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)
		local train2 = Train:new(nil, "mytrain2", Train.COMMUTER, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)

		local track = Track:new{vector=Vector:new(W, NE)}
		track:occupy(train1, train1:direction())
		local vector = track:occupy(train2, train2:direction())
		assertEquals(vector, nil)
		assertEquals(train1.state, Train.CRASHED)
		assertEquals(train1:speed(), TrainType.STOP)
		assertEquals(train2.state, Train.CRASHED)
		assertEquals(train2:speed(), TrainType.STOP)
	end

	function TestTrack.test_unoccupy()
		local track = Track:new{vector=Vector:new(W, NE)}

		local train1 = Train:new(nil, "mytrain1", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)

		local train2 = Train:new(nil, "mytrain2", Train.INTERCITY, TrainType.FAST, Train.PRESENT,  Train.MOVING,
			{ TrainBlock:new(Coord:new(2, 2), Vector:new(W, E), track) }, 1)

		track:occupy(train1, train1:direction())

		assertEquals(track:unoccupy(train1), true)
		assertEquals(track:unoccupy(train2), false)
	end

TestCrossover = {}

	function TestCrossover.test_occupy()
		local crossover = Crossover:new{vectors={Vector:new(W, E), Vector:new(SW, NE)}}
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(NE, SW), track) }, 1)

		local vector = crossover:occupy(train)
		assertEquals(crossover.occupier, train)
		assertEquals(vector, Vector:new(NE, SW))
	end

TestJunctionTrack = {}

	function TestJunctionTrack.test_is_switched()
		local junction_track = JunctionTrack:new({Vector:new(W, NE)}, {Vector:new(W, E)})
		assertEquals(junction_track:is_switched(), true)

		junction_track = JunctionTrack:new({Vector:new(W, E)}, {Vector:new(W, NE)})
		assertEquals(junction_track:is_switched(), false)
	end

TestJunction = {}

	function TestJunction.test_switch_points()
		local track1 = JunctionTrack:new({Vector:new(W, NE)}, {Vector:new(W, E)})
		local track2 = JunctionTrack:new({Vector:new(W, E)}, {Vector:new(W, NE)})
		track1.next = track2
		track2.next = track1

		local junction = Junction:new(track1, true)
		local success = junction:switch_points()
		assertEquals(success, true)
		assertEquals(junction.track, track2)

		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FULL, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(SE, W), track) }, 1)
		junction:occupy(train)
		success = junction:switch_points()
		assertEquals(success, false)
		assertEquals(junction.track, track2)
	end

	function TestJunction.test_auto_switch_points()
		local track1 = JunctionTrack:new({Vector:new(W, NE)}, {Vector:new(W, E)})
		local track2 = JunctionTrack:new({Vector:new(W, E)}, {Vector:new(W, NE)})
		track1.next = track2
		track2.next = track1
		local junction = Junction:new(track1, true)
		local state, vector = junction:auto_switch_points(W)
		assertEquals(state, track2)
		assertEquals(vector, Vector:new(E, W))

		state, vector = junction:auto_switch_points(N)
		assertEquals(state, nil)
		assertEquals(vector, nil)
	end

	function TestJunction.test_occupy_normal()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FULL, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), track) }, 1)

		local track1 = JunctionTrack:new({Vector:new(W, NE)}, {Vector:new(W, E)})
		local track2 = JunctionTrack:new({Vector:new(W, E)}, {Vector:new(W, NE)})
		track1.next = track2
		track2.next = track1
		local junction = Junction:new(track1, true)
		junction:occupy(train)
		assertEquals(junction.occupier, train)
		assertEquals(junction.track, track1)
		assertEquals(train:speed(), TrainType.FAST)
	end

	function TestJunction.test_occupy_point_switch_slow()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.SLOW, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(E, W), track) }, 1)

		local junction, track1, track2 = TestJunction.move_auto_point_switch(train)
		assertEquals(junction.occupier, train)
		assertEquals(junction.track, track2)
		assertEquals(train.state, Train.MOVING)
	end

	function TestJunction.test_occupy_point_switch_fast()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.FAST, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(SE, W), track) }, 1)
		local junction, track1 = TestJunction.move_auto_point_switch(train)
		assertEquals(junction.occupier, nil)
		assertEquals(junction.track, track1)
		assertEquals(train.state, Train.DERAILED)
	end

	function TestJunction.test_unoccupy()
		-- TODO
	end

	function TestJunction.move_auto_point_switch(train)
		local track1 = JunctionTrack:new({Vector:new(W, NE)}, {Vector:new(W, E)})
		local track2 = JunctionTrack:new({Vector:new(W, E)}, {Vector:new(W, NE)})
		track1.next = track2
		track2.next = track1
		local junction = Junction:new(track1, true)
		junction:occupy(train)
		return junction, track1, track2
	end

TestFlyover = {}

	function TestFlyover.test_occupy()
		local train = Train:new(nil, "mytrain", Train.INTERCITY, TrainType.SLOW, Train.PRESENT, Train.MOVING,
			{ TrainBlock:new(Coord:new(1, 1), Vector:new(S, NE), track) }, 1)

		local track1 = Track:new{vector=Vector:new(SE, NW)}
		local track2 = Track:new{vector=Vector:new(SW, NE)}

		local flyover = Flyover:new({track1, track2})
		local vector = flyover:occupy(train)

		assertEquals(flyover.layers[2].occupier, train)
		assertEquals(vector, Vector:new(SW, NE))
	end

	function TestFlyover.test_unoccupy()
		-- TODO
	end