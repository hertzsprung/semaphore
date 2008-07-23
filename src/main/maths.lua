--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

math.e = math.exp(1)

function math.fact(n)
	if n == 0 then
		return 1
	else
		return n * math.fact(n - 1)
	end
end

NormalDist = {}

	function NormalDist:new(mean, stddev)
		local o = {
			mean = mean,
			stddev = stddev
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function NormalDist:point(x)
		return math.e ^ ((-((x - self.mean)^2) / (2 * (self.stddev^2)))) / (self.stddev * math.sqrt(2 * math.pi))
	end

	function NormalDist:random()
		local v = (math.random()*2-1) + (math.random()*2-1) + (math.random()*2-1)
		return v * self.stddev + self.mean
	end