--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('compass')
require('train')
require('map')

TestTrain = {}

	function TestTrain:testShift()
		local new_tail = TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}, nil)

		local t = Train:new(
			'test',
			MOVING,
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
			'test',
			STOPPED,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}, nil),
				TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}, nil),
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E}, nil)
			}
		)
		print(t)
		t:reverse()
		print(t)
		-- TODO: assert
	end

	function TestTrain:testMove()
		local map = Map:new(5, 3)
		local tile1 = map:set(1, 1, TILES[10])
		local tile2 = map:set(2, 1, TILES[10])
		local tile3 = map:set(3, 1, TILES[10])
		local tile4 = map:set(4, 1, TILES[15])

		local t = Train:new(
			'test',
			MOVING,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}, tile3.layers[1]),
				TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}, tile2.layers[1]),
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E}, tile1.layers[1])
			}
		)

		t:move(map)
		print(t)
		-- TODO: assert
	end

LuaUnit:run()