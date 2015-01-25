#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_tiles(sem_render_context* ctx, sem_world* world);
void render_tile(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile);
void render_train(sem_render_context* ctx, sem_train* train);

void sem_render_world(sem_render_context* ctx, sem_world* world) {
	render_tiles(ctx, world);
	render_train(ctx, world->train);
}

void render_tiles(sem_render_context* ctx, sem_world* world) {
	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			render_tile(ctx, i, j, sem_tile_at(world, i, j));
		}
	}
}

void render_tile(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile) {
	#pragma unused(tile)

	cairo_rectangle(ctx->cr, x, y, 1.0, 1.0);	
	cairo_set_source_rgb(ctx->cr, 0.0, 0.53125, 0.26525);
	cairo_fill_preserve(ctx->cr);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);
}

void render_train(sem_render_context* ctx, sem_train* train) {
	cairo_rectangle(ctx->cr, train->x, train->y, 1.0, 1.0);	
	cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 1.0);
	cairo_fill(ctx->cr);
}
