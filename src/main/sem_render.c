#include <math.h>
#include <stdlib.h>
#include <cairo.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_tiles(sem_render_context* ctx, sem_world* world);
void render_tile(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_tile_blank(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_track(sem_render_context* ctx, sem_coordinate coord, sem_track* track);
void render_tile_points(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_tile_signal(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_signal_main(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_signal_aspect(sem_render_context* ctx, sem_tile* tile);
void render_signal_circle(sem_render_context* ctx, sem_tile* tile, double offset);
void render_train(sem_render_context* ctx, sem_train* train);
void render_track_path(sem_render_context* ctx, sem_coordinate coord, sem_track* track);

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
			render_tile(ctx, coord, sem_tile_at(world, i, j));
		}
	}
}

// TODO: rather than passing in coord we could use a cairo transform so that all renders happen from 0-1
// but I'm not sure how efficient this would be, or how convenient?
void render_tile(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
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
	case SIGNAL:
		render_tile_signal(ctx, coord, tile);
	}
}

void render_tile_blank(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	#pragma unused(tile)
	cairo_rectangle(ctx->cr, coord.x, coord.y, 1.0, 1.0);	
	cairo_set_line_width(ctx->cr, 0.01);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.3, 0.0);
	cairo_stroke(ctx->cr);
}

void render_track(sem_render_context* ctx, sem_coordinate coord, sem_track* track) {
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

void render_tile_points(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	for (uint8_t i=0; i<3; i++) {
		sem_track* track = tile->points[i];
		if (track != NULL && track != tile->track) {
			render_track(ctx, coord, track);
		}
	}
	render_track(ctx, coord, tile->track);
}

void render_tile_signal(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	render_track(ctx, coord, tile->track);
	render_signal_main(ctx, coord, tile);
}

void render_signal_main(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	cairo_save(ctx->cr);
	cairo_set_line_width(ctx->cr, 0.05);
	cairo_translate(ctx->cr, coord.x + 0.5, coord.y + 0.5);
	cairo_rotate(ctx->cr, sem_compass_rotation(tile->track->start));

	render_signal_circle(ctx, tile, 0.26);
	render_signal_circle(ctx, tile, -0.26);
	cairo_move_to(ctx->cr, 0, -0.1);
	cairo_line_to(ctx->cr, 0, 0.1);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);

	cairo_restore(ctx->cr);
}

void render_signal_circle(sem_render_context* ctx, sem_tile* tile, double offset) {
	cairo_arc(ctx->cr, 0, offset, 0.18, 0, 2*M_PI);
	render_signal_aspect(ctx, tile);
	cairo_fill_preserve(ctx->cr);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);
}

void render_signal_aspect(sem_render_context* ctx, sem_tile* tile) {
	switch (tile->signal->aspect) {
	case GREEN:
		cairo_set_source_rgb(ctx->cr, 0.0, 1.0, 0.0);
		return;
	case AMBER:
		cairo_set_source_rgb(ctx->cr, 1.0, 0.75, 0.0);
		return;	
	case RED:
		cairo_set_source_rgb(ctx->cr, 1.0, 0.2, 0.2);
		return;
	}
}

void render_train(sem_render_context* ctx, sem_train* train) {
	for (uint32_t i=0; i < train->cars->tail_idx; i++) {
		sem_car* car = (sem_car*) train->cars->items[i];
		render_track_path(ctx, car->position, car->track);
		if (train->state == CRASHED) {
			cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 0.0);
		} else if (train->state == DERAILED) {
			cairo_set_source_rgb(ctx->cr, 1.0, 1.0, 0.0);
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

void render_track_path(sem_render_context* ctx, sem_coordinate coord, sem_track* track) {
	cairo_move_to(ctx->cr, coord.x + 0.5 + SEM_COMPASS_X(track->start)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->start)/2.0);
	if (sem_compass_straight(track->start, track->end)) {
		cairo_line_to(ctx->cr, coord.x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
	} else {
		cairo_curve_to(ctx->cr,
			coord.x + 0.5, coord.y + 0.5,
			coord.x + 0.5 + SEM_COMPASS_X(track->end)*0.25, coord.y + 0.5 + SEM_COMPASS_Y(track->end)*0.25,
			coord.x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
	}
}
