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
			'test',
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
		tile.layers[1]:set_occupier(train)
		assertEquals(tile.layers[1].occupier, train)
		assertEquals(tile.type.layers[1].occupier, nil)
	end

	function TestTile:testOccupied()
		local train1 = Train:new(
			'test1',
			STOPPED,
			{
				TrainBlock:new({3, 1}, Vector:new{W, E}),
				TrainBlock:new({2, 1}, Vector:new{W, E}),
				TrainBlock:new({1, 1}, Vector:new{W, E})
			}
		)

		local train2 = Train:new(
			'test2',
			STOPPED,
			{
				TrainBlock:new({3, 1}, Vector:new{W, E}),
				TrainBlock:new({2, 1}, Vector:new{W, E}),
				TrainBlock:new({1, 1}, Vector:new{W, E})
			}
		)

		local type = TileType:new(false, {{active = {Vector:new{SW, NE}}, inactive = {}}, {active = {Vector:new{SE, NW}}}})
		local tile = Tile:new(type)
		assertEquals(tile.occupied, 0)
		assertEquals(tile.layers[1].occupier, nil)
		
		tile.layers[1]:set_occupier(train1)
		assertEquals(tile.occupied, 1)
		assertEquals(tile.layers[1].occupier, train1)
		
		tile.layers[2]:set_occupier(train2)
		assertEquals(tile.occupied, 2)
		assertEquals(tile.layers[2].occupier, train2)

		tile.layers[1]:set_occupier(nil)
		assertEquals(tile.occupied, 1)
		assertEquals(tile.layers[1].occupier, nil)

		tile.layers[1]:set_occupier(nil)
		assertEquals(tile.occupied, 1)
	end

LuaUnit:run()