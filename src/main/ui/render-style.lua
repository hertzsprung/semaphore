--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('ui.color')
require('compass')

RenderStyle = {
	canvas = Color:new{r=00, g=0.53125, b=0.26525},
	stroke_width = 0.05, -- all basic strokes

	track_back = Color.BLACK,
	track_back_width = 0.2,
	track_front = Color:new{r=0.53125, g=0.796875, b=0.796875},
	track_front_width = 0.1,

	points = Color:new{r=0.2, g=1, b=1},

	intersection_stroke_width = 0.025,
	intersection_length = 0.1,

	signal_main_radius = 0.18,
	signal_main_offset = 0.26,
	signal_main_manual_extra_radius = 0.0625,
	signal_main_manual_extra_offset = Coord:new(0.35, 0.26),
	signal_main_manual_rect_size = Coord:new(0.35*2, 0.26*2),
	signal_main_manual_rect_radius = 0.35,
	signal_sub_fudge = 0.01,
	signal_sub_major_width = 0.3,
	signal_sub_major_height = 0.2,
	signal_sub_minor_width = 0.22,
	signal_sub_minor_height = 0.15,
	signal_red = Color:new{r=1, g=0.2, b=0.2},
	signal_amber = Color:new{r=1, g=0.75, b=0},
	signal_green = Color:new{r=0, g=1, b=0},

	underpass = Color:new{r=0.0664, g=0.332, b=0.0064},
	overpass = Color:new{r=0.2, g=0.73, b=0.465}
}
