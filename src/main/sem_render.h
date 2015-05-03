#ifndef SEM_RENDER_H
#define SEM_RENDER_H

#include <cairo.h>

#include "sem_world.h"

typedef struct {
	cairo_t* cr;
	double scale;
} sem_render_context;

void sem_render_world(sem_render_context* ctx, sem_world* world);

#endif
