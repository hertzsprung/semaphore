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
	local t = Train:new{
		state = STOPPED,
		length=3,
		blocks = {
			{position={3, 1}, vector=Vector:new{W, E}},
			{position={2, 1}, vector=Vector:new{W, E}},
			{position={1, 1}, vector=Vector:new{W, E}}
		}
	}
	print(t)
	t:shift{position={4, 1}, vector=Vector:new{W, NE}}
	print(t)
end

LuaUnit:run()