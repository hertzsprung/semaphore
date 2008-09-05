--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

local Compass = {}

	function Compass.__eq(o1, o2)
		return o1.x == o2.x and o1.y == o2.y
	end
	
	function Compass.__tostring(o)
		return o.name
	end
	
	function Compass:new(x, y, name)
		local o = {
			x = x,
			y = y,
			name = name
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

N  = Compass:new( 0, 1, 'N')
NE = Compass:new( 1, 1, 'NE')
E  = Compass:new( 1, 0, 'E')
SE = Compass:new( 1,-1, 'SE')
S  = Compass:new( 0,-1, 'S')
SW = Compass:new(-1,-1, 'SW')
W  = Compass:new(-1, 0, 'W')
NW = Compass:new(-1, 1, 'NW')
CENTRE = Compass:new(0, 0, 'CENTRE')

N.inverse  = S
NE.inverse = SW
E.inverse  = W
SE.inverse = NW
S.inverse  = N
SW.inverse = NE
W.inverse  = E
NW.inverse = SE

X = 1
Y = 2

Vector = {
	ENTRY = 1,
	EXIT  = 2
}

	local vector_cache = {}
	setmetatable(vector_cache, {__mode = "kv"})

	function Vector.__eq(o1, o2)
		return o1[1] == o2[1] and o1[2] == o2[2]
	end
	
	function Vector.__tostring(o)
		return tostring(o[1]) .. '-' .. tostring(o[2])
	end
	
	function Vector:new(from, to)
		local key = from.name .. '-' .. to.name
		local cache = vector_cache[key]
		if cache_item then
			return cache_item
		else
			local o = {from, to}
			setmetatable(o, self)
			self.__index = self
			vector_cache[key] = o
			return o
		end
	end
	
	-- TODO: rename to invert()
	function Vector:inverse()
		local tmp = self[2]
		self[2] = self[1]
		self[1] = tmp
		return self
	end

	function Vector:get_inverse()
		return Vector:new(self[2], self[1])
	end
	
	function Vector:is_straight()
		return self[1].inverse == self[2]
	end

Coord = {}

	function Coord.__eq(o1, o2)
		return o1.x == o2.x and o1.y == o2.y
	end
	
	function Coord.__tostring(o)
		return '(' .. tostring(o.x) .. ',' .. tostring(o.y) .. ')'
	end
	
	function Coord:new(x, y)
		local o = {
			x = x,
			y = y
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Coord:add(v)
		return Coord:new(self.x + v.x, self.y + v.y)
	end

	function Coord.bounding_box(coords)
		local min, max
		for k, coord in ipairs(coords) do
			if min == nil then
				min = Coord:new(coord.x, coord.y)
				max = Coord:new(coord.x, coord.y)
			else 
				if coord.x < min.x then min.x = coord.x end
				if coord.y < min.y then min.y = coord.y end
				if coord.x > max.x then max.x = coord.x end
				if coord.y > max.y then max.y = coord.y end
			end
		end
		return min, max
	end