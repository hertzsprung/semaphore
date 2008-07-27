--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

require('ltime')

Timer = {}

	function Timer:new(now, multiplier)
		local o = {
			now = now or Timer.millis(),
			multiplier = multiplier or 1
		}
		setmetatable(o, self)
		self.__index = self
		o.last_real_time = o.now
		return o
	end

	function Timer:time()
		local new_real_time = Timer.millis()
		local delta = new_real_time - self.last_real_time
		self.now = self.now + delta * self.multiplier
		self.last_real_time = new_real_time
		logger:info(delta .. "ms since last timer call")
		return self.now, delta
	end

	function Timer.millis()
		-- TODO: want to compensate for timezone?
		s, ms = ltime.gettimeofday()
		return s * 1000 + ms
	end

