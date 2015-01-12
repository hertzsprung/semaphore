#ifndef _SEM_RENDER_H
#define _SEM_RENDER_H

#include <cairo.h>

#include "sem_train.h"

typedef struct {
	cairo_t* cr;
	double scale;
} sem_render_context;

void sem_render_train(sem_render_context* ctx, sem_train* train);

#endif
