--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('compass')
require('train')
require('game')
require('map')

TestTrainType = {}
	function TestTrainType:testSpeed()
		assertEquals(Train.INTERCITY.speeds[TrainType.FAST], 2)
	end

TestTrain = {}

	function TestTrain:testShift()
		local new_tail = TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), nil)

		local t = Train:new{
			TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), nil),
			new_tail,
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), nil),

			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state = Train.MOVING,
			visible_head = 1,
			visible_tail = 3
		}

		local new_head = TrainBlock:new(Coord:new(4, 1), Vector:new(W, NE), nil)

		t:shift(new_head)
		assertEquals(t:head(), new_head)
		assertEquals(t:tail(), new_tail)
	end
	
	function TestTrain:testReverse()
		local t = Train:new{
			TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), nil),
			TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), nil),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), nil),

			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.STOP,
			presence = Train.PRESENT,
			state = Train.STOPPED
		}
		t:reverse()
		assertEquals(t[1], TrainBlock:new(Coord:new(1, 1), Vector:new(E, W), nil))
		assertEquals(t[2], TrainBlock:new(Coord:new(2, 1), Vector:new(E, W), nil))
		assertEquals(t[3], TrainBlock:new(Coord:new(3, 1), Vector:new(E, W), nil))
	end

	function TestTrain:testMove()
		local map = Map:new(5, 3)
		local actions = ActionList:new()
		local timer = Timer:new(1)
		local game = Game:new{
			map = map,
			actions = actions,
			timer = timer
		}
		
		local tile1 = map:set(1, 1, Track:new{vector=Vector:new(W, E)})
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new(W, E)})
		local tile3 = map:set(3, 1, Track:new{vector=Vector:new(W, E)})
		local tile4 = map:set(4, 1, Track:new{vector=Vector:new(W, NE)})

		local t = Train:new{
			TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), tile3),
			TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1),

			map = map,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state = Train.MOVING
		}

		tile1.occupier = t
		tile2.occupier = t
		tile3.occupier = t

		t:move(actions, 1, 1)
		print(t)
		-- TODO: asserts
		local tick = 0
		while tick < 10 do
			game:update()
			tick = tick + 1
		end
	end

	function TestTrain:testEntry()
		local actions = ActionList:new()
		local map = Map:new(5, 3)

		local tile1 = map:set(1, 1, Track:new{vector=Vector:new(W, E)}) -- spawn point
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new(W, E)})
		local tile3 = map:set(3, 1, Track:new{vector=Vector:new(W, E)})
		local tile4 = map:set(4, 1, Track:new{vector=Vector:new(W, NE)})
		local tile5 = map:set(5, 2, Track:new{vector=Vector:new(SW, N)})

		local start_block = TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1)

		local t = Train:new{
			start_block,
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1),

			map = map,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.ENTERING,
			state = Train.MOVING
		}

		tile1.occupier = t

		local second_block = TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2)

		t:move(actions, 1, 1)
		assertEquals(t[1], second_block)
		assertEquals(t[2], start_block)
		assertEquals(t[3], start_block)
		assertEquals(t.visible_tail, 1)

		local third_block = TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), tile3)

		t:move(actions, 2, 2)
		assertEquals(t[1], third_block)
		assertEquals(t[2], second_block)
		assertEquals(t[3], start_block)
		assertEquals(t.visible_tail, 2)
		assertEquals(t.presence, Train.ENTERING)

		t:move(actions, 3, 3)

		local fourth_block = TrainBlock:new(Coord:new(4, 1), Vector:new(W, NE), tile4)
		assertEquals(t[1], fourth_block)
		assertEquals(t[2], third_block)
		assertEquals(t[3], second_block)
		assertEquals(t.visible_tail, 3)
		assertEquals(t.presence, Train.PRESENT)

		t:move(actions, 4, 4)

		assertEquals(t[1], TrainBlock:new(Coord:new(5, 2), Vector:new(SW, N), tile5))
		assertEquals(t[2], fourth_block)
		assertEquals(t[3], third_block)
		assertEquals(t.visible_tail, 3)
	end