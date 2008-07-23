--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('maths')

TestMath = {}

	function TestMath:test_fact()
		assertEquals(math.fact(3), 6)
	end

TestNormalDist = {}

	function TestNormalDist:setUp()
		self.normal_dist = NormalDist:new(2, 5)
	end

	function TestNormalDist:test_random()
		print(self.normal_dist:random())
	end

LuaUnit:run()