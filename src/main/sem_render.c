#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_train(sem_render_context* ctx, sem_train* train);

void sem_render_world(sem_render_context* ctx, sem_world* world) {
	render_train(ctx, world->train);
}

void render_train(sem_render_context* ctx, sem_train* train) {
	cairo_rectangle(ctx->cr, train->x, train->y, 1.0, 1.0);	
	cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 1.0);
	cairo_stroke(ctx->cr);
}
