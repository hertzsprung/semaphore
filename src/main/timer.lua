--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'log'
local logger = log.logger

require('ltime')

Timer = {}

	function Timer:new(now, multiplier)
		local o = {
			now = now or Timer.millis(),
			multiplier = multiplier or 1
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Timer:time()
		local new_real_time = Timer.millis()
		self.last_real_time = self.last_real_time or new_real_time
		local delta = new_real_time - self.last_real_time
		self.now = self.now + delta * self.multiplier
		self.last_real_time = new_real_time
		logger:debug(delta .. "ms since last timer call")
		return self.now, delta
	end

	function Timer.millis()
		-- TODO: want to compensate for timezone?
		s, us = ltime.gettimeofday()
		return s * 1000 + math.floor(us / 1000)
	end

