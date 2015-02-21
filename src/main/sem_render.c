#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_tiles(sem_render_context* ctx, sem_world* world);
void render_tile(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile);
void render_tile_blank(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile);
void render_track(sem_render_context* ctx, sem_coordinate* coord, sem_track* track);
void render_tile_points(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile);
void render_train(sem_render_context* ctx, sem_train* train);
void render_track_path(sem_render_context* ctx, sem_coordinate* coord, sem_track* track);

void sem_render_world(sem_render_context* ctx, sem_world* world) {
	render_tiles(ctx, world);
	for (uint32_t i=0; i < world->trains->tail_idx; i++) {
		render_train(ctx, world->trains->items[i]);
	}
}

void render_tiles(sem_render_context* ctx, sem_world* world) {
	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			sem_coordinate coord;
			sem_coordinate_set(&coord, i, j);
			render_tile(ctx, &coord, sem_tile_at(world, i, j));
		}
	}
}

// TODO: rather than passing in coord we could use a cairo transform so that all renders happen from 0-1
// but I'm not sure how efficient this would be, or how convenient?
void render_tile(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile) {
	switch(tile->class) {
	case BLANK:
		render_tile_blank(ctx, coord, tile);
		return;
	case TRACK:
		render_track(ctx, coord, tile->track);
		return;
	case POINTS:
		render_tile_points(ctx, coord, tile);
		return;
	}
}

void render_tile_blank(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile) {
	#pragma unused(tile)
	cairo_rectangle(ctx->cr, coord->x, coord->y, 1.0, 1.0);	
	cairo_set_line_width(ctx->cr, 0.01);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.3, 0.0);
	cairo_stroke(ctx->cr);
}

void render_track(sem_render_context* ctx, sem_coordinate* coord, sem_track* track) {
	sem_track* t = track;
	do {
		render_track_path(ctx, coord, t);
		cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
		cairo_set_line_width(ctx->cr, 0.2);
		cairo_stroke_preserve(ctx->cr);
		cairo_set_source_rgb(ctx->cr, 0.53125, 0.796875, 0.796875);
		cairo_set_line_width(ctx->cr, 0.1);
		cairo_stroke(ctx->cr);

		t = t->next;
	} while (t != NULL);
}

void render_tile_points(sem_render_context* ctx, sem_coordinate* coord, sem_tile* tile) {
	for (uint8_t i=0; i<3; i++) {
		sem_track* track = tile->points[i];
		if (track != NULL && track != tile->track) {
			render_track(ctx, coord, track);
		}
	}
	render_track(ctx, coord, tile->track);
}

void render_train(sem_render_context* ctx, sem_train* train) {
	for (uint32_t i=0; i < train->cars->tail_idx; i++) {
		sem_coordinate* c = (sem_coordinate*) train->cars->items[i];
		sem_track* track = sem_tile_at_coord(train->world, c)->track;
		render_track_path(ctx, c, track);
		if (train->state == CRASHED) {
			cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 0.0);
		} else {
			cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 1.0);
		}
		if (i == 0) {
			cairo_set_line_width(ctx->cr, 0.6);
		} else {
			cairo_set_line_width(ctx->cr, 0.4);
		}
		cairo_stroke(ctx->cr);
	}
}

void render_track_path(sem_render_context* ctx, sem_coordinate* coord, sem_track* track) {
	cairo_move_to(ctx->cr, coord->x + 0.5 + SEM_COMPASS_X(track->start)/2.0, coord->y + 0.5 + SEM_COMPASS_Y(track->start)/2.0);
	cairo_line_to(ctx->cr, coord->x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord->y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
}
