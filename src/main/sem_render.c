#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"

void sem_render_train(sem_render_context* ctx, sem_train* train) {
	cairo_rectangle(ctx->cr, train->x, train->y, 1.0, 1.0);	
	cairo_set_source_rgb(ctx->cr, rand()/(double)RAND_MAX, 1.0, 1.0);
	cairo_stroke(ctx->cr);
}
