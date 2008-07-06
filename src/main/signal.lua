--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

Signal = {
	MAIN_AUTO   = 1,
	MAIN_MANUAL = 2,
	SUB         = 3,
	GREEN       = 1,
	AMBER       = 2,
	RED         = 3
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