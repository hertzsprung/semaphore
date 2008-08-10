--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('map')
require('tile')

TestMap = {}

    function TestMap:setUp()
		self.map = Map:new(5, 3)
    end

	function TestMap:testEmptyCell()
		assertEquals(self.map[2][4], BLANK)
	end

	function TestMap:testOutsideWidth()
		assertEquals(self.map[2][6], nil)
	end

	function TestMap:testBadKey()
		assertEquals(self.map[2]['shoes'], nil)
	end

	function TestMap:testSet()
		local tile_in = Track:new(Vector:new{W, E})
		local tile_out = self.map:set(1, 2, tile_in)
		assertEquals(self.map[2][1], tile_out)
	end
