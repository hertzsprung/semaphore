#include <math.h>
#include <stdlib.h>
#include <cairo.h>
#include <stdio.h>

#include "sem_render.h"
#include "sem_train.h"
#include "sem_world.h"

void render_tiles(sem_render_context* ctx, sem_world* world);
void render_tile(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_tile_blank(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_track(sem_render_context* ctx, sem_coordinate coord, sem_track* track);
void render_tile_points(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_points_highlight(sem_render_context* ctx, sem_coordinate coord, sem_track* track);
void render_tile_signal(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile);
void render_signal_main(sem_render_context* ctx, sem_tile* tile);
void render_signal_main_manual(sem_render_context* ctx, sem_tile* tile);
void render_rounded_rect(sem_render_context* ctx, double x, double y, double w, double h, double r);
void render_signal_sub(sem_render_context* ctx, sem_tile* tile);
void render_signal_aspect(sem_render_context* ctx, sem_signal_aspect aspect);
void render_signal_aspect_unstroked(sem_render_context* ctx, sem_signal_aspect aspect);
void render_signal_set_aspect(sem_render_context* ctx, sem_signal_aspect aspect);
void render_signal_circle(sem_render_context* ctx, double offset_x, double offset_y, double radius);
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
		return;
	}
}

void render_tile_blank(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	#pragma unused(tile)
	cairo_rectangle(ctx->cr, coord.x, coord.y, 1.0, 1.0);	
	cairo_set_line_width(ctx->cr, 0.01);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.45, 0.1);
	cairo_stroke(ctx->cr);
}

void render_track(sem_render_context* ctx, sem_coordinate coord, sem_track* track) {
	sem_track* t = track;
	do {
		render_track_path(ctx, coord, t);
		cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
		cairo_set_line_width(ctx->cr, ctx->style->track_back_width);
		cairo_stroke(ctx->cr);

		t = t->next;
	} while (t != NULL);

	t = track;
	do {
		render_track_path(ctx, coord, t);
		cairo_set_source(ctx->cr, ctx->style->track_front_color);
		cairo_set_line_width(ctx->cr, ctx->style->track_front_width);
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
	if (!sem_track_straight(tile->track)) {
		render_points_highlight(ctx, coord, tile->track);
	}
}

void render_points_highlight(sem_render_context* ctx, sem_coordinate coord, sem_track* track) {
	unit_vector corner = sem_track_corner(track);
	double corner_x = 0.5 + SEM_COMPASS_X(corner)/2.0;
	double corner_y = 0.5 + SEM_COMPASS_Y(corner)/2.0;
	double start_x = 0.5 + SEM_COMPASS_X(track->start)/2.0;
	double start_y = 0.5 + SEM_COMPASS_Y(track->start)/2.0;
	double end_x = 0.5 + SEM_COMPASS_X(track->end)/2.0;
	double end_y = 0.5 + SEM_COMPASS_Y(track->end)/2.0;

	cairo_save(ctx->cr);
	cairo_set_line_cap(ctx->cr, CAIRO_LINE_CAP_ROUND);
	cairo_move_to(ctx->cr, coord.x + (start_x + corner_x)/2.0, coord.y + (start_y + corner_y)/2.0);
	cairo_curve_to(ctx->cr,
		coord.x + (0.5 + start_x + 1.5*corner_x)/3.5, coord.y + (0.5 + start_y + 1.5*corner_y)/3.5,
		coord.x + (0.5 + end_x + 1.5*corner_x)/3.5, coord.y + (0.5 + end_y + 1.5*corner_y)/3.5,
		coord.x + (end_x + corner_x)/2.0, coord.y + (end_y + corner_y)/2.0);

	cairo_set_source(ctx->cr, ctx->style->points_highlight_color);
	cairo_set_line_width(ctx->cr, ctx->style->points_highlight_width);
	cairo_stroke(ctx->cr);
	cairo_restore(ctx->cr);
}

void render_tile_signal(sem_render_context* ctx, sem_coordinate coord, sem_tile* tile) {
	render_track(ctx, coord, tile->track);

	cairo_save(ctx->cr);
	cairo_set_line_width(ctx->cr, 0.05);
	cairo_translate(ctx->cr, coord.x + 0.5, coord.y + 0.5);
	cairo_rotate(ctx->cr, sem_compass_rotation(tile->track->start));

	switch (tile->signal->type) {
	case MAIN_AUTO:
		render_signal_main(ctx, tile);
		break;
	case MAIN_MANUAL:
		render_signal_main_manual(ctx, tile);	
		render_signal_main(ctx, tile);
		break;
	case SUB:
		render_signal_sub(ctx, tile);
		break;
	}

	cairo_move_to(ctx->cr, 0, -ctx->style->track_back_width/2.0);
	cairo_line_to(ctx->cr, 0, ctx->style->track_back_width/2.0);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);

	cairo_restore(ctx->cr);
}

void render_signal_main(sem_render_context* ctx, sem_tile* tile) {
	double offset = ctx->style->signal_main_offset;
	double radius = ctx->style->signal_main_radius;

	render_signal_circle(ctx, 0, offset, radius);
	render_signal_aspect(ctx, tile->signal->aspect);
	render_signal_circle(ctx, 0, -offset, radius);
	render_signal_aspect(ctx, tile->signal->aspect);
}

void render_signal_main_manual(sem_render_context* ctx, sem_tile* tile) {
	double width = ctx->style->signal_main_manual_rect_width;
	double height = ctx->style->signal_main_manual_rect_height;
	render_rounded_rect(ctx, -width/2, -height/2, width, height, ctx->style->signal_main_manual_rect_radius);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);

	double offset_x = ctx->style->signal_main_manual_extra_offset_x;
	double offset_y = ctx->style->signal_main_manual_extra_offset_y;
	double radius = ctx->style->signal_main_manual_extra_radius;

	render_signal_circle(ctx, offset_x, offset_y, radius);
	render_signal_aspect_unstroked(ctx, tile->signal->aspect);
	render_signal_circle(ctx, -offset_x, offset_y, radius);
	render_signal_aspect_unstroked(ctx, tile->signal->aspect);
	render_signal_circle(ctx, offset_x, -offset_y, radius);
	render_signal_aspect_unstroked(ctx, tile->signal->aspect);
	render_signal_circle(ctx, -offset_x, -offset_y, radius);
	render_signal_aspect_unstroked(ctx, tile->signal->aspect);
}


/*
 A****BQ
H      C
*      *
G      D
 F****E
*/
void render_rounded_rect(sem_render_context* ctx, double x, double y, double w, double h, double r) {
	cairo_move_to(ctx->cr, x+r,y);                      // Move to A
	cairo_line_to(ctx->cr, x+w-r,y);                    // Straight line to B
	cairo_curve_to(ctx->cr, x+w,y,x+w,y,x+w,y+r);       // Curve to C, Control points are both at Q
	cairo_line_to(ctx->cr, x+w,y+h-r);                  // Move to D
	cairo_curve_to(ctx->cr, x+w,y+h,x+w,y+h,x+w-r,y+h); // Curve to E
	cairo_line_to(ctx->cr, x+r,y+h);                    // Line to F
	cairo_curve_to(ctx->cr, x,y+h,x,y+h,x,y+h-r);       // Curve to G
	cairo_line_to(ctx->cr, x,y+r);                      // Line to H
	cairo_curve_to(ctx->cr, x,y,x,y,x+r,y);             // Curve to A
}

void render_signal_sub(sem_render_context* ctx, sem_tile* tile) {
	double w, h;
	if (tile->signal->aspect == GREEN) {
		w = ctx->style->signal_sub_minor_width;
		h = ctx->style->signal_sub_minor_height;
	} else {
		w = ctx->style->signal_sub_major_width;
		h = ctx->style->signal_sub_major_height;
	}

	cairo_rectangle(ctx->cr, 
		-w/2.0,
		-h - ctx->style->track_back_width/2.0 + 0.025,
		w,
		h);
	render_signal_aspect(ctx, tile->signal->aspect);

	cairo_rectangle(ctx->cr, 
		-w/2.0,
		ctx->style->track_back_width/2.0 - 0.025,
		w,
		h);
	render_signal_aspect(ctx, tile->signal->aspect == RED ? AMBER : tile->signal->aspect);
}

void render_signal_circle(sem_render_context* ctx, double offset_x, double offset_y, double radius) {
	cairo_arc(ctx->cr, offset_x, offset_y, radius, 0, 2*M_PI);
}

void render_signal_aspect(sem_render_context* ctx, sem_signal_aspect aspect) {
	render_signal_set_aspect(ctx, aspect);
	cairo_fill_preserve(ctx->cr);
	cairo_set_source_rgb(ctx->cr, 0.0, 0.0, 0.0);
	cairo_stroke(ctx->cr);
}

void render_signal_aspect_unstroked(sem_render_context* ctx, sem_signal_aspect aspect) {
	render_signal_set_aspect(ctx, aspect);
	cairo_fill(ctx->cr);
}

void render_signal_set_aspect(sem_render_context* ctx, sem_signal_aspect aspect) {
	switch (aspect) {
	case GREEN:
		cairo_set_source(ctx->cr, ctx->style->signal_green);
		break;
	case AMBER:
		cairo_set_source(ctx->cr, ctx->style->signal_amber);
		break;
	case RED:
		cairo_set_source(ctx->cr, ctx->style->signal_red);
		break;
	}
}

void render_train(sem_render_context* ctx, sem_train* train) {
	sem_car* car = train->head_car;
	while (car != NULL) {
		render_track_path(ctx, car->position, car->track);
		if (train->state == CRASHED) {
			cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 0.0);
		} else if (train->state == DERAILED) {
			cairo_set_source_rgb(ctx->cr, 1.0, 1.0, 0.0);
		} else {
			cairo_set_source_rgb(ctx->cr, 1.0, 0.0, 1.0);
		}
		if (car == train->head_car) {
			cairo_set_line_width(ctx->cr, 0.4);
		} else {
			cairo_set_line_width(ctx->cr, 0.25);
		}
		cairo_stroke(ctx->cr);
		car = car->next;
	}
}

void render_track_path(sem_render_context* ctx, sem_coordinate coord, sem_track* track) {
	cairo_move_to(ctx->cr, coord.x + 0.5 + SEM_COMPASS_X(track->start)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->start)/2.0);
	if (sem_track_straight(track)) {
		cairo_line_to(ctx->cr, coord.x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
	} else {
		cairo_curve_to(ctx->cr,
			coord.x + 0.5, coord.y + 0.5,
			coord.x + 0.5 + SEM_COMPASS_X(track->end)*0.25, coord.y + 0.5 + SEM_COMPASS_Y(track->end)*0.25,
			coord.x + 0.5 + SEM_COMPASS_X(track->end)/2.0, coord.y + 0.5 + SEM_COMPASS_Y(track->end)/2.0);
	}
}

sem_success sem_render_default_style(sem_render_style* style) {
	style->canvas = cairo_pattern_create_rgb(0.0, 0.53125, 0.26525);
	style->track_front_color = cairo_pattern_create_rgb(0.53125, 0.796875, 0.796875);
	style->points_highlight_color = cairo_pattern_create_rgb(0.2, 1.0, 1.0);
	style->signal_red = cairo_pattern_create_rgb(1.0, 0.2, 0.2);
	style->signal_amber = cairo_pattern_create_rgb(1.0, 0.75, 0.0);
	style->signal_green = cairo_pattern_create_rgb(0.0, 1.0, 0.0);
	
	style->track_back_width = 0.2;
	style->track_front_width = 0.1;
	style->points_highlight_width = 0.05;
	style->signal_main_radius = 0.18;
	style->signal_main_offset = 0.26;
	style->signal_main_manual_extra_radius = 0.0625;
	style->signal_main_manual_extra_offset_x = 0.35;
	style->signal_main_manual_extra_offset_y = 0.26;
	style->signal_main_manual_rect_width = style->signal_main_manual_extra_offset_x*2;
	style->signal_main_manual_rect_height = style->signal_main_manual_extra_offset_y*2;
	style->signal_main_manual_rect_radius = 0.35;
	style->signal_sub_major_width = 0.3;
	style->signal_sub_major_height = 0.2;
	style->signal_sub_minor_width = 0.22;
	style->signal_sub_minor_height = 0.15;
	return SEM_OK;
}
