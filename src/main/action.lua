--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

ActionList = {}

function ActionList:new()
	local o = {
		latest_time = 0
	}
	setmetatable(o, self)
	self.__index = self
	return o
end

function ActionList:add(time, action)
	self[time] = self[time] or {}
	self[time][#self[time]+1] = action
end

function ActionList:execute(end_time)
	local start_time = self.latest_time + 1
	logger:debug("Executing all actions between " .. start_time .. " and " .. end_time .. " inclusive")
	for t = start_time, end_time do
		for i, action in ipairs(self[t] or {}) do
			action()
		end
	end
	self.latest_time = end_time
end