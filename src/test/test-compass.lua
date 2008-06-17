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

	function TestCompass:testEqualsSameObject()
		assert(NW == NW)
	end

	function TestCompass:testEqualsCompassComponents()
		local northwest = Compass:new{-1, 1}
		assert(NW == northwest)
	end

LuaUnit:run()