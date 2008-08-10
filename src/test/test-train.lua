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
		local new_tail = TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}, nil)

		local t = Train:new(
			nil,
			'test',
			Train.INTERCITY,
			TrainType.FULL,
			Train.MOVING,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}, nil),
				new_tail,
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E}, nil)
			}
		)

		local new_head = TrainBlock:new(Vector:new{4, 1}, Vector:new{W, NE}, nil)

		t:shift(new_head)
		assertEquals(t:head(), new_head)
		assertEquals(t:tail(), new_tail)
	end
	
	function TestTrain:testReverse()
		local t = Train:new(
			nil,
			'test',
			Train.INTERCITY,
			TrainType.STOP,
			Train.STOPPED,
			{
				TrainBlock:new(Coord:new(3, 1), Vector:new{W, E}, nil),
				TrainBlock:new(Coord:new(2, 1), Vector:new{W, E}, nil),
				TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, nil)
			}
		)
		print(t)
		t:reverse()
		print(t)
		-- TODO: assert
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
		
		local tile1 = map:set(1, 1, Track:new{vector=Vector:new{W, E}})
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new{W, E}})
		local tile3 = map:set(3, 1, Track:new{vector=Vector:new{W, E}})
		local tile4 = map:set(4, 1, Track:new{vector=Vector:new{W, NE}})

		local t = Train:new(
			map,
			'test',
			Train.INTERCITY,
			TrainType.FULL,
			Train.MOVING,
			{
				TrainBlock:new(Coord:new(3, 1), Vector:new{W, E}, tile3),
				TrainBlock:new(Coord:new(2, 1), Vector:new{W, E}, tile2),
				TrainBlock:new(Coord:new(1, 1), Vector:new{W, E}, tile1)
			}
		)

		tile1.occupier = t
		tile2.occupier = t
		tile3.occupier = t

		t:move(actions, 1, 1, game)
		print(t)
		local tick = 0
		while tick < 10 do
			game:update()
			tick = tick + 1
		end
	end
