#!/usr/bin/env lua
--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

log = {}
log.logger = logging.console()

log.logger:setLevel(logging.DEBUG)
