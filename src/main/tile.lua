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

TILES={}
TILES[0]  = TileType:new(false, {})
TILES[8]  = TileType:new(false, {Vector:new{SW, NE}, Vector:new{SE, NW}})
TILES[10] = TileType:new(false, {Vector:new{W, E}})
TILES[11] = TileType:new(false, {Vector:new{S, N}})
TILES[12] = TileType:new(false, {Vector:new{SE, NW}})
TILES[13] = TileType:new(false, {Vector:new{SW, NE}})
TILES[14] = TileType:new(false, {Vector:new{SE, W}})
TILES[15] = TileType:new(false, {Vector:new{W, NE}})
TILES[16] = TileType:new(false, {Vector:new{NW, E}})
TILES[17] = TileType:new(false, {Vector:new{SW, E}})

TILES[40] = TileType:new(true, {{active = {Vector:new{W, E}}, inactive = {Vector:new{W, NE}}}})

TILES[50] = TileType:new(true, {{active = {Vector:new{W, NE}}, inactive= {Vector:new{W, E}}}})

TILES[40].next = TILES[50]
TILES[50].next = TILES[40]

ENTRY = 1
EXIT  = 2

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

	for i=1, #type.layers do
		local mt = {}
		function mt.__index(o, key)
			if key == 'occupied' then
				return rawget(o, key)
			else
				return rawget(type.layers[i], key)
			end
		end

		o.layers[i] = {}
		o.layers[i].occupied = nil -- train occupying the layer
		setmetatable(o.layers[i], mt)
	end
	setmetatable(o, self)
	return o
end

-- TODO: move this to Train, and accept a Map as an argument
function Tile:route(train, tile)
	local loco = train.blocks[1]
	for i, layer in ipairs(tile.layers) do
		for j, route in ipairs(layer.active) do
			if route[ENTRY] == loco[EXIT].inverse then
				return layer, route
			end
			if route[EXIT]  == loco[EXIT].inverse then
				return layer, Vector:new{route[EXIT], route[ENTRY]}
			end
		end
	end
end
