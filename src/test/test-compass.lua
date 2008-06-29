--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('compass')

TestCompass = {}

	function TestCompass:testInverse()
		local inverse = NW.inverse
		assertEquals(inverse[1], 1)
		assertEquals(inverse[2], -1)
	end

TestVector = {}
	
	function TestVector:testInverse()
		local inverse = Vector:new{W, E}:inverse()
		assert(inverse[1] == E)
		assert(inverse[2] == W)
	end

	function TestVector:testAdd()
		local p1 = Vector:new{3, 2}
		local p2 = p1:add(Vector:new{1, -1})
		assertEquals(p2[1], 4)
		assertEquals(p2[2], 1)
	end

LuaUnit:run()


