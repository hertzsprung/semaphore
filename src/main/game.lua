--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')

Game = {}
	
	function Game:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Game:update()
		local now = self.timer:time()
		self.actions:execute(now)
	end