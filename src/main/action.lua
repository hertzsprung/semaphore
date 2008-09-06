--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

require 'heap'

local logger = logging.console()

ActionList = {}

	function ActionList:new(o)
		o = o or {
			latest_time = 0,
			heap = Heap:new()
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	-- If time isn't specified, add action for immediate execution
	function ActionList:add(action, time)
		self.heap:insert{key=time or self.latest_time, action=action}
	end

	function ActionList:execute(end_time)
		local start_time = self.latest_time + 1
		logger:debug("Executing all actions between " .. start_time .. " and " .. end_time .. " inclusive")
		while self.heap[1] and self.heap[1].key <= end_time do
			local heap_item = self.heap:delete_min()
			heap_item.action(self, heap_item.key, end_time)
		end
		self.latest_time = end_time
	end