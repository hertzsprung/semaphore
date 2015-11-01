#ifndef SEM_RENDER_H
#define SEM_RENDER_H

#include <cairo.h>

#include "sem_game.h"
#include "sem_world.h"

typedef struct {
	cairo_pattern_t* canvas;
	cairo_pattern_t* track_front_color;
	cairo_pattern_t* points_highlight_color;
	cairo_pattern_t* buffer_color;
	cairo_pattern_t* signal_red;
	cairo_pattern_t* signal_amber;
	cairo_pattern_t* signal_green;
	cairo_pattern_t* station_color;

	double track_front_width;
	double track_back_width;
	double station_front_width;
	double depot_back_width;
	double depot_front_width;
	double points_highlight_width;
	double signal_main_radius;
	double signal_main_offset;
	double signal_sub_major_width;
	double signal_sub_major_height;
	double signal_sub_minor_width;
	double signal_sub_minor_height;
	double signal_main_manual_extra_radius;
	double signal_main_manual_extra_offset_x;
	double signal_main_manual_extra_offset_y;
	double signal_main_manual_rect_width;
	double signal_main_manual_rect_height;
	double signal_main_manual_rect_radius;
	double buffer_size;
} sem_render_style;

typedef struct {
	cairo_t* cr;
	sem_render_style* style;
	double scale;
} sem_render_context;

void sem_render_game(sem_render_context* ctx, sem_game* game);

sem_success sem_render_default_style(sem_render_style* style);

#endif
