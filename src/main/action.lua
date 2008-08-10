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

function ActionList:new(o)
	o = o or {
		latest_time = 0
	}
	setmetatable(o, self)
	self.__index = self
	return o
end

function ActionList:add(time, action)
	self[time] = self[time] or {}
	table.insert(self[time], action)
end

function ActionList:execute(end_time)
	local start_time = self.latest_time + 1
	logger:debug("Executing all actions between " .. start_time .. " and " .. end_time .. " inclusive")
	for t = start_time, end_time do
		if self[t] then
			for i, action in ipairs(self[t]) do
				action(self, t, end_time)
			end
			self[t] = nil
		end
	end
	self.latest_time = end_time
end