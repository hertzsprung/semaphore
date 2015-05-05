#ifndef SEM_RENDER_H
#define SEM_RENDER_H

#include <cairo.h>

#include "sem_world.h"

typedef struct {
	cairo_pattern_t* canvas;
} sem_render_style;

typedef struct {
	cairo_t* cr;
	sem_render_style* style;
	double scale;
} sem_render_context;

void sem_render_world(sem_render_context* ctx, sem_world* world);

sem_success sem_render_default_style(sem_render_style* style);

#endif
