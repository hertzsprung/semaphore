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
require('station')

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
			actions = actions,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state = Train.MOVING
		}

		tile1.occupier = t
		tile2.occupier = t
		tile3.occupier = t

		t:move(1, 1)
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
			actions = actions,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.ENTERING,
			state = Train.MOVING
		}

		tile1.occupier = t

		local second_block = TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2)

		t:move(1, 1)
		assertEquals(t[1], second_block)
		assertEquals(t[2], start_block)
		assertEquals(t[3], start_block)
		assertEquals(t.visible_tail, 1)

		local third_block = TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), tile3)

		t:move(2, 2)
		assertEquals(t[1], third_block)
		assertEquals(t[2], second_block)
		assertEquals(t[3], start_block)
		assertEquals(t.visible_tail, 2)
		assertEquals(t.presence, Train.ENTERING)

		t:move(3, 3)

		local fourth_block = TrainBlock:new(Coord:new(4, 1), Vector:new(W, NE), tile4)
		assertEquals(t[1], fourth_block)
		assertEquals(t[2], third_block)
		assertEquals(t[3], second_block)
		assertEquals(t.visible_tail, 3)
		assertEquals(t.presence, Train.PRESENT)

		t:move(4, 4)

		assertEquals(t[1], TrainBlock:new(Coord:new(5, 2), Vector:new(SW, N), tile5))
		assertEquals(t[2], fourth_block)
		assertEquals(t[3], third_block)
		assertEquals(t.visible_tail, 3)
	end

	function TestTrain:testExit()
		local actions = ActionList:new()
		local map = Map:new(5, 3)

		local tile1 = map:set(1, 1, Track:new{vector=Vector:new(W, E)}) -- spawn point
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new(W, E)})
		local tile3 = map:set(3, 1, Track:new{vector=Vector:new(W, E)})
		local tile4 = map:set(4, 1, SpawnExit:new{compass=W})

		local first_tile = TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1)
		local second_tile = TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2)
		local third_tile = TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), tile3)
		local t = Train:new{
			third_tile,
			second_tile,
			first_tile,

			map = map,
			actions = actions,
			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state = Train.MOVING,
			visible_head = 1,
			visible_tail = 3
		}

		tile1.occupier = t
		tile2.occupier = t
		tile3.occupier = t

		local fourth_tile = TrainBlock:new(Coord:new(4, 1), Vector:new(W, CENTRE), tile4)

		t:move(1, 1)
		assertEquals(t.presence, Train.EXITING)
		assertEquals(t.visible_head, 2)
		assertEquals(t[1], fourth_tile)
		assertEquals(t[2], third_tile)
		assertEquals(t[3], second_tile)
		assertEquals(first_tile.occupier, nil)

		t:move(2, 2)
		assertEquals(t.presence, Train.EXITING)
		assertEquals(t.visible_head, 3)
		assertEquals(t[1], fourth_tile)
		assertEquals(t[2], fourth_tile)
		assertEquals(t[3], third_tile)
		assertEquals(second_tile.occupier, nil)

		t:move(3, 3)
		assertEquals(t.presence, Train.ABSENT)
		assertEquals(t.visible_head, 4) -- nothing is visible
		assertEquals(t[1], fourth_tile)
		assertEquals(t[2], fourth_tile)
		assertEquals(t[3], fourth_tile)
		assertEquals(third_tile.occupier, nil)
	end