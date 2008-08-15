--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'luaunit'
require 'action'

TestActionList = {}

	function TestActionList:testExecute()
		local run = 1
		local al = ActionList:new()
		al:add(1, function () run = run * 3 end)
		al:add(2, function () run = run + 1 end)
		al:add(4, function () run = run * 100 end)

		al:execute(3)

		assertEquals(run, 4)
	end
