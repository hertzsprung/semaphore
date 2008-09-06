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
		al:add(function () run = run * 3 end, 1)
		al:add(function () run = run + 1 end, 2)
		al:add(function () run = run * 100 end, 4)

		al:execute(3)

		assertEquals(run, 4)
	end
