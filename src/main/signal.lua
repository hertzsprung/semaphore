--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('train')

Signal = {
	MAIN_AUTO   = 1,
	MAIN_MANUAL = 2,
	SUB         = 3,
	GREEN       = 1,
	AMBER       = 2,
	RED         = 3,
}

local MAIN = 1 -- used internally to represent either MAIN_AUTO or MAIN_MANUAL
local SUB  = 2

Signal.SPEEDS = {
	[TrainType.FULL] = {
		[MAIN] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.SLOW },
			[Signal.RED]   = { speed=TrainType.STOP, emergency=true }
		},
		[SUB] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.FAST },
			[Signal.RED]   = { speed=TrainType.SLOW }
		}
	},

	[TrainType.FAST] = {
		[MAIN] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.SLOW },
			[Signal.RED]   = { speed=TrainType.STOP }
		},
		[SUB] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.FAST },
			[Signal.RED]   = { speed=TrainType.SLOW }
		}
	},

	[TrainType.SLOW] = {
		[MAIN] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.SLOW },
			[Signal.RED]   = { speed=TrainType.STOP }
		},
		[SUB] = {
			[Signal.GREEN] = { speed=TrainType.FULL },
			[Signal.AMBER] = { speed=TrainType.FAST },
			[Signal.RED]   = { speed=TrainType.STOP }
		}
	}
}

	function Signal:new(type, aspect)
		local o = {
			type = type,
			aspect = aspect,
			previous_aspect = aspect
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Signal:set_aspect(aspect)
		self.previous_aspect = self.aspect
		self.aspect = aspect
	end

	function Signal:set_to_previous_aspect()
		local tmp = self.aspect
		self.aspect = self.previous_aspect
		self.previous_aspect = tmp
	end

	function Signal:next_speed(train)
		local type
		if self.type == Signal.SUB then type = SUB else type = MAIN end
		local s = Signal.SPEEDS[train:speed()][type][self.aspect]
		return s.speed, s.emergency or false
	end