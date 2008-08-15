--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'luaunit'
require 'heap'

TestHeap = {}

	function TestHeap.test_heap()
		local heap = Heap:new()
		local iterations = 10

		for i=1,iterations do
			heap:insert{key=math.random(1, iterations), value="test"}
		end
		
		for i=1,iterations do
			heap:delete_min()
		end
	end
