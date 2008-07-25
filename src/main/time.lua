--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('ltime')

time = {}

function time.millis()
	s, ms = ltime.gettimeofday()
	return s * 1000 + ms
end