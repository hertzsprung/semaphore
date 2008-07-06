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

LuaUnit:run()