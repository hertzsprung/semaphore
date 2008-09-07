--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('compass')
require('tile')
local cairo = require("lcairo")
local PI = math.pi

local function set_color(ctx, color)
	cairo.set_source_rgb(ctx, color.r, color.g, color.b)
end

local function rounded_rect(ctx,x,y,w,h,r)
    --[[
		 A****BQ
		H      C
		*      *
		G      D
		 F****E
	]]--
	r = r or 10
	cairo.move_to(ctx, x+r,y)                      -- Move to A
	cairo.line_to(ctx, x+w-r,y)                    -- Straight line to B
	cairo.curve_to(ctx, x+w,y,x+w,y,x+w,y+r)       -- Curve to C, Control points are both at Q
	cairo.line_to(ctx, x+w,y+h-r)                  -- Move to D
	cairo.curve_to(ctx, x+w,y+h,x+w,y+h,x+w-r,y+h) -- Curve to E
	cairo.line_to(ctx, x+r,y+h)                    -- Line to F
	cairo.curve_to(ctx, x,y+h,x,y+h,x,y+h-r)       -- Curve to G
	cairo.line_to(ctx, x,y+r)                      -- Line to H
	cairo.curve_to(ctx, x,y,x,y,x+r,y)             -- Curve to A
end

CairoRender = {}

	function CairoRender:new(o)
		o = o or {}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function CairoRender:v_midpoint(y)
		return self.map.h - y - 0.5
	end

	function CairoRender:render(ctx)
		local canvas = self.style.canvas
		cairo.set_source_rgb(ctx, canvas.r, canvas.g, canvas.b)
		cairo.paint(ctx)

		local track_back = self.style.track_back
		local track_front = self.style.track_front

		local function position(x, y, compass)
			return x + 0.5 + (compass.x * 0.5), self:v_midpoint(y) - (compass.y * 0.5)
		end

		local function draw_tile(x, y, tile)
			local entry = tile.vector[Vector.ENTRY]
			local exit = tile.vector[Vector.EXIT]
			
			cairo.set_line_width(ctx, self.style.track_back_width)
			set_color(ctx, track_back)
			cairo.move_to(ctx, position(x, y, entry))
			cairo.line_to(ctx, position(x, y, exit))
			cairo.stroke(ctx)

			cairo.set_line_width(ctx, self.style.track_front_width)
			set_color(ctx, track_front)
			cairo.move_to(ctx, position(x, y, entry))
			cairo.line_to(ctx, position(x, y, exit))
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

	-- TODO: not sure how we're going to lay out the functions ultimately
	function CairoRender:draw_main_signal(ctx, x, y, signal)
		local style = self.style
		local rotate_table = {
			[N]  = 0.5,
			[NE] = -0.25,
			[E]  = 0,
			[SE] = 0.25,
			[S]  = 0.5,
			[SW] = -0.25,
			[W]  = 0,
			[NW] = 0.25
		}
		local color_table = {
			[Signal.RED] = style.signal_red,
			[Signal.AMBER] = style.signal_amber,
			[Signal.GREEN] = style.signal_green
		}
		local aspect_color = color_table[signal.aspect]
		
		cairo.set_line_width(ctx, style.stroke_width)

		-- rotate the whole thing so that it aligns with track
		cairo.save(ctx)
		cairo.translate(ctx, x + 0.5, self:v_midpoint(y))
		cairo.rotate(ctx, rotate_table[signal.vector[Vector.ENTRY]] * PI)

		if signal.type == Signal.MAIN_MANUAL then
			local rect_size = style.signal_main_manual_rect_size
			rounded_rect(ctx, -rect_size.x/2, -rect_size.y/2, rect_size.x, rect_size.y, style.signal_main_manual_rect_radius)
			set_color(ctx, Color.BLACK)
			cairo.stroke(ctx)

			local offset = style.signal_main_manual_extra_offset
			local radius = style.signal_main_manual_extra_radius
			set_color(ctx, aspect_color)
			cairo.arc(ctx, -offset.x, -offset.y, radius, 0, 2 * PI)
			cairo.fill(ctx)
			cairo.arc(ctx, offset.x, -offset.y, radius, 0, 2 * PI)
			cairo.fill(ctx)
			cairo.arc(ctx, -offset.x, offset.y, radius, 0, 2 * PI)
			cairo.fill(ctx)
			cairo.arc(ctx, offset.x, offset.y, radius, 0, 2 * PI)
			cairo.fill(ctx)
		end

		local function draw_circle(x, y)
			cairo.arc(ctx, x, y, style.signal_main_radius, 0, 2 * PI)
			set_color(ctx, aspect_color)
			cairo.fill_preserve(ctx)
			set_color(ctx, Color.BLACK)
			cairo.stroke(ctx)
		end

		draw_circle(0, style.signal_offset) -- upper circle
		draw_circle(0, -style.signal_offset) -- lower circle

		-- connecting line
		cairo.move_to(ctx, 0, -style.stroke_width)
		cairo.line_to(ctx, 0, style.stroke_width)
		cairo.stroke(ctx)
		cairo.restore(ctx)
	end