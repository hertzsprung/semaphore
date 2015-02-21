#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_tiles(sem_render_context* ctx, sem_world* world);
void render_tile(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile);
void render_tile_blank(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile);
void render_tile_track(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile);
void render_train(sem_render_context* ctx, sem_train* train);
void render_track_path(sem_render_context* ctx, sem_coordinate* coord, sem_track* track);

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

// TODO: rather than passing in x and y we could use a cairo transform so that all renders happen from 0-1
// but I'm not sure how efficient this would be, or how convenient?
void render_tile(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile) {
	switch(tile->class) {
	case BLANK:
		render_tile_blank(ctx, x, y, tile);
		return;
	case TRACK:
		render_tile_track(ctx, x, y, tile);
		return;
	}
}

void render_tile_blank(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile) {
	#pragma unused(tile)
	cairo_rectangle(ctx->cr, x, y, 1.0, 1.0);	
	cairo_set_source_rgb(ctx->cr, 0.0, 0.3, 0.0);
	cairo_stroke(ctx->cr);
}

void render_tile_track(sem_render_context* ctx, uint32_t x, uint32_t y, sem_tile* tile) {
	cairo_move_to(ctx->cr, x + 0.5 + SEM_COMPASS_X(tile->track->start)/2.0, y + 0.5 + SEM_COMPASS_Y(tile->track->start)/2.0);
	cairo_line_to(ctx->cr, x + 0.5 + SEM_COMPASS_X(tile->track->end)/2.0, y + 0.5 + SEM_COMPASS_Y(tile->track->end)/2.0);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(ctx->cr, 0.2);
	cairo_stroke_preserve(ctx->cr);
	cairo_set_source_rgb(ctx->cr, 0.53125, 0.796875, 0.796875);
	cairo_set_line_width(ctx->cr, 0.1);
	cairo_stroke(ctx->cr);
}

void render_train(sem_render_context* ctx, sem_train* train) {
	for (uint32_t i=0; i < train->cars->tail_idx; i++) {
		sem_coordinate* c = (sem_coordinate*) train->cars->items[i];
		sem_track* track = sem_tile_at_coord(train->world, c)->track;
		render_track_path(ctx, c, track);
		cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 1.0);
		cairo_set_line_width(ctx->cr, 0.4);
		cairo_stroke(ctx->cr);
	}
}

void render_track_path(sem_render_context* ctx, sem_coordinate* coord, sem_track* track) {
	cairo_move_to(ctx->cr, coord->x + 0.5 + SEM_COMPASS_X(track->start)/2.0, coord->y + 0.5 + SEM_COMPASS_Y(track->start)/2.0);
	cairo_line_to(ctx->cr, coord->x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord->y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
}
