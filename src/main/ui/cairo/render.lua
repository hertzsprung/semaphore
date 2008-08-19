--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('compass')
require('tile')
local cairo = require("lcairo")

CairoRender = {}

	function CairoRender:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function CairoRender:render(ctx)
		local canvas = self.style.canvas
		cairo.set_source_rgb(ctx, canvas.r, canvas.g, canvas.b)
		cairo.paint(ctx)

		local track_front = self.style.track_front
		local function draw_tile(x, y, tile)
			local entry = tile.vector[Vector.ENTRY]
			local exit = tile.vector[Vector.EXIT]
			
			cairo.set_line_width(ctx, self.style.track_front_width)
			cairo.set_source_rgb(ctx, track_front.r, track_front.g, track_front.b)
			cairo.move_to(ctx, x + 0.5 + (entry.x * 0.5), y + 0.5 + (entry.y * 0.5))
			cairo.line_to(ctx, x + 0.5 + (exit.x * 0.5), y + 0.5 + (exit.y * 0.5))
			cairo.stroke(ctx)
		end

		for i=1,self.map.w do
			for j=1,self.map.h do
				local tile = self.map[j][i]
				if tile ~= BLANK then
					draw_tile(i-1, j-1, tile)
				end
			end
		end
	end