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
		local t = Train:new(
			MOVING,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E})
			}
		)
		print(t)
		t:shift(TrainBlock:new(Vector:new{4, 1}, Vector:new{W, NE}))
		print(t)
		-- TODO: assert
	end
	
	function TestTrain:testReverse()
		local t = Train:new(
			STOPPED,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E})
			}
		)
		print(t)
		t:reverse()
		print(t)
		-- TODO: assert
	end

	function TestTrain:testMove()
		local map = Map:new(5, 3)
		map:set(1, 1, TILES[10])
		map:set(2, 1, TILES[10])
		map:set(3, 1, TILES[10])
		map:set(4, 1, TILES[15])

		local t = Train:new(
			MOVING,
			{
				TrainBlock:new(Vector:new{3, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{2, 1}, Vector:new{W, E}),
				TrainBlock:new(Vector:new{1, 1}, Vector:new{W, E})
			}
		)

		t:move(map)
		print(t)
		-- TODO: assert
	end

LuaUnit:run()