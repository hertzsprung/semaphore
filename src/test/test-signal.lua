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
		self.signal = Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.AMBER}
    end

	function TestSignal:test_tostring()
		print(self.signal)
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
		local main_amber = Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.AMBER}
		local main_red   = Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.RED}
		-- TODO: fix blocks and length
		local full_train = Train:new(nil, "mytrain", Train.COMMUTER, TrainType.FULL, Train.MOVING, {}, 0)

		local speed, emergency = main_amber:next_speed(full_train)
		assertEquals(speed, TrainType.SLOW)
		assertEquals(emergency, false)
		
		local speed, emergency = main_red:next_speed(full_train)
		assertEquals(speed, TrainType.STOP)
		assertEquals(emergency, true)

		-- TODO: a few more permutations
	end

	function TestSignal:test_update_state()
		local train = {}
		
		local signals = {
			Signal:new{type=Signal.SUB, aspect=Signal.GREEN},
			Signal:new{type=Signal.SUB, aspect=Signal.RED},
			Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.GREEN},
			Signal:new{type=Signal.SUB, aspect=Signal.GREEN},
			Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.AMBER},
			Signal:new{type=Signal.SUB, aspect=Signal.GREEN},
			Signal:new{type=Signal.MAIN_MANUAL, aspect=Signal.GREEN},
			Signal:new{type=Signal.SUB, aspect=Signal.GREEN},
			Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.AMBER}
		}

		for i, signal in ipairs(signals) do
			signal:update_state(train)
			str = ''
			for j, s in ipairs(signals) do
				if s.type == Signal.SUB then str = str .. ({'g', 'a', 'r'})[s.aspect] end
				if s.type == Signal.MAIN_AUTO then str = str .. ({'G', 'A', 'R'})[s.aspect] end
				if s.type == Signal.MAIN_MANUAL then str = str .. ({'_G_', '_A_', '_R_'})[s.aspect] end
				str = str .. '...'
			end
			print(str)
			--print('most_recent_sub_signal ', tostring(train.most_recent_sub_signal), 'most_recent_main_signal ', tostring(train.most_recent_main_signal), 'sub_signal_behind_most_recent_main', tostring(train.sub_signal_behind_most_recent_main))
		end
	end