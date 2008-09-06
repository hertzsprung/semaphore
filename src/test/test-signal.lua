--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')

require('action')
require('compass')
require('map')
require('signal')
require('train')

TestSignal = {}

	function TestSignal:setUp()
		self.signal = Signal:new{type=Signal.MAIN_AUTO, aspect=Signal.AMBER}
    end

	function TestSignal:test_tostring()
		assertEquals(tostring(self.signal), '<Signal MAIN_AUTO AMBER>')
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
		-- TODO: fix absence blocks
		local full_train = Train:new{
			name = "mytrain",
			type = Train.COMMUTER,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state = Train.MOVING
		}

		local speed, emergency = main_amber:next_speed(full_train)
		assertEquals(speed, TrainType.SLOW)
		assertEquals(emergency, false)
		
		local speed, emergency = main_red:next_speed(full_train)
		assertEquals(speed, TrainType.STOP)
		assertEquals(emergency, true)
	end

	function TestSignal:test_update_state()
		local train = Train:new{
			name = 'test',
			type= Train.COMMUTER,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state =Train.MOVING
		}
		
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
		end
	end

	function TestSignal:test_occupy_amber_sub()
		local train = Train:new{
			TrainBlock:new(Coord:new(3, 1), Vector:new(W, E), nil),
			TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), nil),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), nil),

			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FULL,
			presence = Train.PRESENT,
			state =Train.MOVING
		}

		local signal = Signal:new{type=Signal.SUB, aspect=Signal.AMBER, vector=Vector:new(W, E)}
		local vector = signal:occupy(train)
		assertEquals(vector, Vector:new(W, E))
		assertEquals(signal.occupier, train)
		assertEquals(train.most_recent_sub_signal, signal)
		assertEquals(signal.aspect, Signal.AMBER)
		assertEquals(train.signal_speed, TrainType.FAST)
		assertEquals(train:speed(), TrainType.FAST)
	end

	function TestSignal:test_occupy_main_red()
		local actions = ActionList:new();
		local map = Map:new(5, 3)

		local tile1 = map:set(1, 1, Track:new{vector=Vector:new(W, E)})
		local tile2 = map:set(2, 1, Track:new{vector=Vector:new(W, NE)})
		local tile3 = map:set(3, 2, Track:new{vector=Vector:new(SW, E)})
		local signal = map:set(4, 2, Signal:new{
			vector=Vector:new(W, E),
			type=Signal.MAIN_AUTO,
			aspect=Signal.RED, vector=Vector:new(W, E),
			actions=actions
		})

		local train = Train:new{
			TrainBlock:new(Coord:new(3, 2), Vector:new(W, E), tile3),
			TrainBlock:new(Coord:new(2, 1), Vector:new(W, E), tile2),
			TrainBlock:new(Coord:new(1, 1), Vector:new(W, E), tile1),

			name = 'test',
			type = Train.INTERCITY,
			signal_speed = TrainType.FAST,
			presence = Train.PRESENT,
			state = Train.MOVING,
			map = map
		}

		tile1.occupier = train
		tile2.occupier = train
		tile3.occupier = train

		local vector = signal:occupy(train)
		assertEquals(vector, nil)
		assertEquals(signal.occupier, nil)
		assertEquals(train:speed(), TrainType.STOP)

		signal:set_aspect(Signal.AMBER)
		actions:execute(1)
		assertEquals(train.state, Train.MOVING)
		assertEquals(train:speed(), TrainType.SLOW)
	end