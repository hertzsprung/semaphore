--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('train')
require('tile')

Signal = Tile:new()
Signal.MAIN_AUTO   = 1
Signal.MAIN_MANUAL = 2
Signal.SUB         = 3
Signal.GREEN       = 1
Signal.AMBER       = 2
Signal.RED         = 3

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

	function Signal.__tostring(o)
		return '<Signal ' .. ({'MAIN_AUTO', 'MAIN_MANUAL', 'SUB'})[o.type] .. ' ' .. ({'GREEN', 'AMBER', 'RED'})[o.aspect] .. '>'
	end

	function Signal:occupy(train)
		-- TODO
	end

	-- when train head actually occupies this tile
	function Signal:update_state(train)
		if self.type == Signal.SUB then
			if self.aspect == Signal.GREEN then
				self:set_aspect(Signal.AMBER)
			end
			if train.most_recent_sub_signal and train.most_recent_sub_signal.aspect == Signal.AMBER then
				train.most_recent_sub_signal:set_aspect(Signal.GREEN)
			end
			train.most_recent_sub_signal = self
		else
			self:set_aspect(Signal.RED)
			if train.most_recent_main_signal and train.most_recent_main_signal.type == Signal.MAIN_AUTO then
				train.most_recent_main_signal:set_to_previous_aspect()
				if train.sub_signal_behind_most_recent_main.aspect == Signal.AMBER then
					train.sub_signal_behind_most_recent_main:set_aspect(Signal.GREEN)
				end
			end
			train.most_recent_main_signal = self
			train.sub_signal_behind_most_recent_main = train.most_recent_sub_signal
			train.most_recent_sub_signal = nil
		end
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
