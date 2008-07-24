--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('station')

TestStation = {}

	function TestStation:test_new()
		local p1 = Platform:new("1")
		local p2 = Platform:new("2")
		local station = Station:new("MYS", "My Station", Station.PASSENGER, {INTERCITY, COMMUTER}, {p1, p2})
	end

LuaUnit:run()