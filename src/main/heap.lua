--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

local floor = math.floor

Heap = {}

	function Heap:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Heap:insert(x)
		local hole = #self + 1
		local half = math.floor(hole/2)
		while hole > 1 and x.key < self[half].key do
			self[hole] = self[half]
			hole = half
			half = math.floor(hole/2)
		end
		self[hole] = x
	end

	function Heap:delete_min()
		if #self == 0 then return end
		local x = self[1]
		self[1] = self[#self]
		table.remove(self, #self)
		self:percolate_down(1)
		return x
	end

	function Heap:percolate_down(hole)
		local child
		local tmp = self[hole]
		while hole * 2 <= #self do
			child = hole * 2
			if child ~= #self and self[child+1].key < self[child].key then
				child = child + 1
			end
			if self[child].key < tmp.key then
				self[hole] = self[child]
			else
				break
			end
			hole = child
		end
		self[hole] = tmp
	end
