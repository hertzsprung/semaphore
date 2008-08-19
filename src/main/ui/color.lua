--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

Color = {
	a=1
}

	function Color:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

Color.BLACK = Color:new{r=0, g=0, b=0, a=1}
