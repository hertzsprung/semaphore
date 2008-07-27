--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('timer')
require('luaunit')

TestTimer = {}

	function TestTimer:test_millis()
		print(Timer.millis())
	end

	function TestTimer:test_time()
		local timer = Timer:new()
		print(timer:time())
	end

LuaUnit:run()