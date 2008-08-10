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
		local action1Run = false
		local action2Run = false
		local action3Run = false
		local al = ActionList:new()
		al:add(1, function () action1Run = true end)
		al:add(2, function () action2Run = true end)
		al:add(4, function () action3Run = true end)

		al:execute(3)

		assertEquals(action1Run, true)
		assertEquals(action2Run, true)
		assertEquals(action3Run, false)
	end
