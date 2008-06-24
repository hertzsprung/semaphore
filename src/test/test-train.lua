--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('compass')
require('train')

TestTrain = {}

	function TestTrain:testShift()
		local t = Train:new(
			MOVING,
			{
				TrainBlock:new({3, 1}, Vector:new{W, E}),
				TrainBlock:new({2, 1}, Vector:new{W, E}),
				TrainBlock:new({1, 1}, Vector:new{W, E})
			}
		)
		print(t)
		t:shift(TrainBlock:new({4, 1}, Vector:new{W, NE}))
		print(t)
	end
	
	function TestTrain:testReverse()
		local t = Train:new(
			STOPPED,
			{
				TrainBlock:new({3, 1}, Vector:new{W, E}),
				TrainBlock:new({2, 1}, Vector:new{W, E}),
				TrainBlock:new({1, 1}, Vector:new{W, E})
			}
		)
		print(t)
		t:reverse()
		print(t)
	end

LuaUnit:run()