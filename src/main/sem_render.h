#ifndef SEM_RENDER_H
#define SEM_RENDER_H

#include <cairo.h>

#include "sem_world.h"

typedef struct {
	cairo_pattern_t* canvas;
	cairo_pattern_t* track_front_color;
	cairo_pattern_t* signal_red;
	cairo_pattern_t* signal_amber;
	cairo_pattern_t* signal_green;

	double track_front_width;
	double track_back_width;
	double signal_main_radius;
	double signal_main_offset;
	double signal_sub_major_width;
	double signal_sub_major_height;
	double signal_sub_minor_width;
	double signal_sub_minor_height;
	double signal_main_manual_extra_radius;
	double signal_main_manual_extra_offset_x;
	double signal_main_manual_extra_offset_y;
} sem_render_style;

typedef struct {
	cairo_t* cr;
	sem_render_style* style;
	double scale;
} sem_render_context;

void sem_render_world(sem_render_context* ctx, sem_world* world);

sem_success sem_render_default_style(sem_render_style* style);

#endif
