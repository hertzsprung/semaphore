--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('map')

TestMap = {}

    function TestMap:setUp()
		self.map = Map:new(5, 3)
    end

	function TestMap:testEmptyCell()
		assertEquals(self.map[2][4], TILES[0])
	end

	function TestMap:testOutsideWidth()
		assertEquals(self.map[2][6], nil)
	end

	function TestMap:testBadKey()
		assertEquals(self.map[2]['shoes'], nil)
	end

	function TestMap:testSet()
		local tile = self.map:set(1, 2, TILES[40])
		assertEquals(self.map[2][1], tile)
		assertEquals(tile.type, TILES[40])
		assertEquals(tile.occupied, 0)
	end

LuaUnit:run()