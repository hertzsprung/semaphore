--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('action')
require('game')
require('timer')

TestGame = {}

	function TestGame:test_update()
		local timer = Timer:new(1)
		local actions = ActionList:new()

		function f(actions, requested_time, actual_time)
			print(requested_time, actual_time)
			actions:add(requested_time + 1, f)
		end
		actions:add(1, f)
		local game = Game:new{
			map = nil,
			actions = actions,
			timer = timer
		}
		--while true do
		--	game:update()
		--end
	end
