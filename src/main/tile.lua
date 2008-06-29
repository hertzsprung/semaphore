--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('compass')

TileType = {}

	function TileType:new(junction, layers)
		local o = {
			junction = junction,
			layers = layers
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

-- FIXME: sort out remaining tiles with active/inactive vectors
-- probably better to come up with some nicer constructors
TILES={}
TILES[0]  = TileType:new(false, {})
TILES[8]  = TileType:new(false, {Vector:new{SW, NE}, Vector:new{SE, NW}})
TILES[10] = TileType:new(false, {{active = {Vector:new{W, E}}, inactive = {}}})
TILES[11] = TileType:new(false, {Vector:new{S, N}})
TILES[12] = TileType:new(false, {Vector:new{SE, NW}})
TILES[13] = TileType:new(false, {Vector:new{SW, NE}})
TILES[14] = TileType:new(false, {Vector:new{SE, W}})
TILES[15] = TileType:new(false, {{active = {Vector:new{W, NE}}, inactive = {}}})
TILES[16] = TileType:new(false, {Vector:new{NW, E}})
TILES[17] = TileType:new(false, {Vector:new{SW, E}})

TILES[40] = TileType:new(true, {{active = {Vector:new{W, E}}, inactive = {Vector:new{W, NE}}}})

TILES[50] = TileType:new(true, {{active = {Vector:new{W, NE}}, inactive= {Vector:new{W, E}}}})

TILES[40].next = TILES[50]
TILES[50].next = TILES[40]

ENTRY = 1
EXIT  = 2

Layer = {}

	function Layer:new(tile, type_layer)
		local o = {
			occupier = nil, -- train occupying the layer
			tile = tile
		}
		local mt = {}
		function mt.__index(o, key)
			return rawget(o, key) or rawget(self, key) or rawget(type_layer, key)
		end	
		setmetatable(o, mt)
		return o
	end
	
	function Layer:set_occupier(train)
		if self.occupier == nil and train ~= nil then
			self.tile.occupied = self.tile.occupied + 1
		elseif self.occupier ~= nil then
			if train == nil then
				self.tile.occupied = self.tile.occupied - 1
			else
				self.tile.occupied = self.tile.occupied + 1
			end
		end
		self.occupier = train
	end

Tile = {}

	function Tile.__index(o, key)
		return rawget(o, key) or rawget(o.type, key)
	end
	
	function Tile:new(type)
		local layers = {}
	
		local o = {
			type = type,
			occupied = 0, -- number of trains occupying the tile
			layers = layers
		}
	
		for i, type_layer in ipairs(type.layers) do
			o.layers[i] = Layer:new(o, type_layer)
		end
		setmetatable(o, self)
		return o
	end

