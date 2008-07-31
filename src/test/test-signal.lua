--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('signal')

TestSignal = {}

	function TestSignal:setUp()
		self.signal = Signal:new(Signal.MAIN_AUTO, Signal.AMBER)
    end

	function TestSignal:test_set_aspect()
		self.signal:set_aspect(Signal.RED)
		assertEquals(self.signal.aspect, Signal.RED)
		assertEquals(self.signal.previous_aspect, Signal.AMBER)

		self.signal:set_to_previous_aspect()
		assertEquals(self.signal.aspect, Signal.AMBER)
		assertEquals(self.signal.previous_aspect, Signal.RED)
	end

	function TestSignal:test_next_speed()
		local main_amber = Signal:new(Signal.MAIN_AUTO, Signal.AMBER)
		local main_red   = Signal:new(Signal.MAIN_AUTO, Signal.RED)
		-- TODO: fix blocks and length
		local full_train = Train:new("mytrain", Train.COMMUTER, TrainType.FULL, Train.MOVING, {}, 0)

		local speed, emergency = main_amber:next_speed(full_train)
		assertEquals(speed, TrainType.SLOW)
		assertEquals(emergency, false)
		
		local speed, emergency = main_red:next_speed(full_train)
		assertEquals(speed, TrainType.STOP)
		assertEquals(emergency, true)

		-- TODO: a few more permutations
	end

LuaUnit:run()