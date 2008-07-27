--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('compass')

TestCompass = {}

	function TestCompass:test_inverse()
		local inverse = NW.inverse
		assertEquals(inverse.x, 1)
		assertEquals(inverse.y, -1)
	end

TestVector = {}
	
	function TestVector:test_inverse()
		local inverse = Vector:new{W, E}:inverse()
		assert(inverse[1] == E)
		assert(inverse[2] == W)
	end

	function TestVector:test_is_straight()
		local p1 = Vector:new{W, E}
		assert(p1:is_straight())
		
		local p2 = Vector:new{W, NE}
		assertEquals(p2:is_straight(), false)
	end

TestCoord = {}

	function TestCoord:test_add()
		local p1 = Coord:new(3, 2)
		local p2 = p1:add(Coord:new(1, -1))
		assertEquals(p2.x, 4)
		assertEquals(p2.y, 1)
	end

	function TestCoord:test_bounding_box()
		local c = {
			Coord:new(1, 6),
			Coord:new(5, 4),
			Coord:new(3, 7)
		}
		local min, max = Coord.bounding_box(c)
		assertEquals(min, Coord:new(1, 4))
		assertEquals(max, Coord:new(5, 7))
	end

LuaUnit:run()


