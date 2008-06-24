--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('tile')
require('train')

TestTile = {}

	function TestTile:testNew()
		local train = Train:new(
			STOPPED,
			{
				TrainBlock:new({3, 1}, Vector:new{W, E}),
				TrainBlock:new({2, 1}, Vector:new{W, E}),
				TrainBlock:new({1, 1}, Vector:new{W, E})
			}
		)

		local type = TileType:new(true, {{active = {Vector:new{W, E}}, inactive = {Vector:new{W, NE}}}})
		local tile = Tile:new(type)

		assertEquals(tile.type, type)
		assert(tile.junction)
		assert(tile.layers[1].active[1] == Vector:new{W, E})
		tile.layers[1].occupied = train
		assert(tile.layers[1].occupied == train)
		assert(tile.type.layers[1].occupied == nil)
	end

LuaUnit:run()