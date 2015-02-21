#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <cairo.h>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "sem_action_list.h"
#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_input.h"
#include "sem_render.h"
#include "sem_timer.h"
#include "sem_train.h"
#include "sem_world.h"

int main(/*int argc, char **argv*/) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sem_set_error("Unable to initialize SDL: %s", SDL_GetError());
		return sem_fatal_error();
	}
	atexit(SDL_Quit);

	SDL_Window* window;
	window = SDL_CreateWindow("semaphore", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == NULL) {
		sem_set_error("Unable to create window: %s", SDL_GetError());
		return sem_fatal_error();
	}

	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL) {
		sem_set_error("Unable to create renderer: %s", SDL_GetError());
		return sem_fatal_error();
	}

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	SDL_Texture *texture;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);
	if (texture == NULL) {
		sem_set_error("Unable to create texture: %s", SDL_GetError());
		return sem_fatal_error();
	}

	void *pixels;
	int pitch;

	if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
		sem_set_error("Failed to lock texture: %s", SDL_GetError());
		return sem_fatal_error();
	}

	cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
		pixels,
		CAIRO_FORMAT_ARGB32,
		width, height, pitch);

	cairo_t *cr = cairo_create(cairo_surface);

	SDL_UnlockTexture(texture);

	sem_render_context render_ctx;
	render_ctx.cr = cr;	
	render_ctx.scale = 32.0;

	// train 1
	sem_train train1;
	sem_train_init(&train1);
	sem_coordinate train1Car1;
	sem_coordinate_set(&train1Car1, 2, 0);
	sem_train_add_car(&train1, &train1Car1);

	sem_coordinate train1Car2;
	sem_coordinate_set(&train1Car2, 1, 0);
	sem_train_add_car(&train1, &train1Car2);

	sem_coordinate train1Car3;
	sem_coordinate_set(&train1Car3, 0, 1);
	sem_train_add_car(&train1, &train1Car3);

	train1.direction = SEM_EAST;

	// train 2
	sem_train train2;
	sem_train_init(&train2);
	sem_coordinate train2Car1;
	sem_coordinate_set(&train2Car1, 10, 0);
	sem_train_add_car(&train2, &train2Car1);

	sem_coordinate train2Car2;
	sem_coordinate_set(&train2Car2, 11, 0);
	sem_train_add_car(&train2, &train2Car2);

	sem_coordinate train2Car3;
	sem_coordinate_set(&train2Car3, 12, 1);
	sem_train_add_car(&train2, &train2Car3);

	train2.direction = SEM_WEST;

	// train 3
	sem_train train3;
	sem_train_init(&train3);
	sem_coordinate train3Car1;
	sem_coordinate_set(&train3Car1, 2, 8);
	sem_train_add_car(&train3, &train3Car1);

	sem_coordinate train3Car2;
	sem_coordinate_set(&train3Car2, 1, 8);
	sem_train_add_car(&train3, &train3Car2);

	sem_coordinate train3Car3;
	sem_coordinate_set(&train3Car3, 0, 8);
	sem_train_add_car(&train3, &train3Car3);

	train3.direction = SEM_EAST;

	// train 4
	sem_train train4;
	sem_train_init(&train4);
	sem_coordinate train4Car1;
	sem_coordinate_set(&train4Car1, 13, 8);
	sem_train_add_car(&train4, &train4Car1);

	sem_coordinate train4Car2;
	sem_coordinate_set(&train4Car2, 14, 8);
	sem_train_add_car(&train4, &train4Car2);

	sem_coordinate train4Car3;
	sem_coordinate_set(&train4Car3, 15, 8);
	sem_train_add_car(&train4, &train4Car3);

	train4.direction = SEM_WEST;

	sem_world world;
	world.max_x = 64;
	world.max_y = 64;
	sem_world_init_blank(&world);

	sem_world_add_train(&world, &train1);
	sem_world_add_train(&world, &train2);
	sem_world_add_train(&world, &train3);
	sem_world_add_train(&world, &train4);

	for (uint32_t i=0; i<=1; i++) {
		sem_tile* t = sem_tile_at(&world, i*8 + 1, 0);
		t->class = TRACK;
		sem_track track_SW_E;
		track_SW_E.start = SEM_SOUTH | SEM_WEST;
		track_SW_E.end = SEM_EAST;
		t->track = &track_SW_E;

		t = sem_tile_at(&world, i*8 + 3, 0);
		t->class = TRACK;
		sem_track track_W_SE;
		track_W_SE.start = SEM_WEST;
		track_W_SE.end = SEM_SOUTH | SEM_EAST;
		t->track = &track_W_SE;

		t = sem_tile_at(&world, i*8, 1);
		t->class = TRACK;
		sem_track track_S_NE;
		track_S_NE.start = SEM_SOUTH;
		track_S_NE.end = SEM_NORTH | SEM_EAST;
		t->track = &track_S_NE;

		t = sem_tile_at(&world, i*8 + 4, 1);
		t->class = TRACK;
		sem_track track_S_NW;
		track_S_NW.start = SEM_SOUTH;
		track_S_NW.end = SEM_NORTH | SEM_WEST;
		t->track = &track_S_NW;

		t = sem_tile_at(&world, i*8, 3);
		t->class = TRACK;
		sem_track track_N_SE;
		track_N_SE.start = SEM_NORTH;
		track_N_SE.end = SEM_SOUTH | SEM_EAST;
		t->track = &track_N_SE;

		t = sem_tile_at(&world, i*8 + 4, 3);
		t->class = TRACK;
		sem_track track_N_SW;
		track_N_SW.start = SEM_NORTH;
		track_N_SW.end = SEM_SOUTH | SEM_WEST;
		t->track = &track_N_SW;

		t = sem_tile_at(&world, i*8 + 1, 4);
		t->class = TRACK;
		sem_track track_NW_E;
		track_NW_E.start = SEM_NORTH | SEM_WEST;
		track_NW_E.end = SEM_EAST;
		t->track = &track_NW_E;

		t = sem_tile_at(&world, i*8 + 3, 4);
		t->class = TRACK;
		sem_track track_W_NE;
		track_W_NE.start = SEM_WEST;
		track_W_NE.end = SEM_NORTH | SEM_EAST;
		t->track = &track_W_NE;

		// straight track

		t = sem_tile_at(&world, i*8 + 2, 0);
		t->class = TRACK;
		sem_track trackEW;
		trackEW.start = SEM_EAST;
		trackEW.end = SEM_WEST;
		t->track = &trackEW;

		t = sem_tile_at(&world, i*8 + 2, 4);
		t->class = TRACK;
		t->track = &trackEW;

		t = sem_tile_at(&world, i*8, 2);
		t->class = TRACK;
		sem_track trackNS;
		trackNS.start = SEM_NORTH;
		trackNS.end = SEM_SOUTH;
		t->track = &trackNS;

		t = sem_tile_at(&world, i*8 + 4, 2);
		t->class = TRACK;
		t->track = &trackNS;
	}

	sem_track trackNW_SE;
	trackNW_SE.start = SEM_NORTH | SEM_WEST;
	trackNW_SE.end = SEM_SOUTH | SEM_EAST;

	sem_tile* t = sem_tile_at(&world, 6, 2);
	t->class = TRACK;
	t->track = &trackNW_SE;

	sem_track trackW_SE;
	trackW_SE.start = SEM_WEST;
	trackW_SE.end = SEM_SOUTH | SEM_EAST;

	t = sem_tile_at(&world, 5, 1);
	t->class = TRACK;
	t->track = &trackW_SE;

	sem_track trackNW_E;
	trackNW_E.start = SEM_NORTH | SEM_WEST;
	trackNW_E.end = SEM_EAST;

	t = sem_tile_at(&world, 7, 3);
	t->class = TRACK;
	t->track = &trackNW_E;

	sem_track trackNW_S;
	trackNW_S.start = SEM_NORTH | SEM_WEST;
	trackNW_S.end = SEM_SOUTH;

	t = sem_tile_at(&world, 4, 1);
	t->class = POINTS;
	t->track = &trackNW_S;
	t->points[0] = &trackNW_S;
	t->points[1] = &trackNW_E;
	t->points[2] = NULL;

	sem_track trackN_SE;
	trackN_SE.start = SEM_NORTH;
	trackN_SE.end = SEM_SOUTH | SEM_EAST;

	t = sem_tile_at(&world, 8, 3);
	t->class = POINTS;
	t->track = &trackN_SE;
	t->points[0] = &trackN_SE;
	t->points[1] = &trackW_SE;
	t->points[2] = NULL;

	// straight collision track

	sem_track trackEW;
	trackEW.start = SEM_EAST;
	trackEW.end = SEM_WEST;

	for (uint32_t i=0; i<16; i++) {
		sem_tile* ts = sem_tile_at(&world, i, 8);
		ts->class = TRACK;
		ts->track = &trackEW;
	}

	sem_timer_context timer_ctx;
	timer_ctx.now = 0L;
	timer_ctx.multiplier = 1.0;
	timer_ctx.clock = sem_clock_monotonic;
	sem_timer_init(&timer_ctx);

	sem_dynamic_array actions;
	sem_heap_init(&actions);

	cairo_scale(cr, render_ctx.scale, render_ctx.scale);

	SDL_Event e;
	bool quit = false;	
	uint64_t frames = 0;
	while (!quit) {
		sem_timer_now(&timer_ctx);

		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_LCTRL || e.key.keysym.sym == SDLK_RCTRL) {
					// ignore
				} else {
					quit = true;
				}
			}
			if (e.type == SDL_MOUSEWHEEL) {
				if (SDL_GetModState() & KMOD_CTRL) {
					if (e.wheel.y > 0) {
						timer_ctx.multiplier *= 1.1 * e.wheel.y;
					} else if (e.wheel.y < 0) {
						timer_ctx.multiplier /= -1.1 * e.wheel.y;
					}
				} else {
					if (e.wheel.y > 0) {
						cairo_scale(cr, 1.1 * e.wheel.y, 1.1 * e.wheel.y);
					} else if (e.wheel.y < 0) {
						cairo_scale(cr, 1.0 / (1.1 * -e.wheel.y), 1.0 / (1.1 * -e.wheel.y));
					}
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP) {
				double x = e.button.x;
				double y = e.button.y;
				cairo_device_to_user(cr, &x, &y);

				sem_coordinate coord;
				sem_coordinate_set(&coord, (uint32_t) (floor(x)), (uint32_t) (floor(y)));
				sem_input_event input;
				input.time = timer_ctx.now;
				input.tile = &coord;

				sem_action* a = NULL;

				// TODO: wrap up train/tile input handlers into one big input routine
				sem_train_input_act_upon(&input, &world, &a);

				if (a != NULL) {
					// TODO: should just chuck this onto the action list with "immediate" flag set
					a->function(&actions, a);
				} else {
					sem_tile_input_act_upon(&input, &world, &a);
					if (a != NULL) a->function(&actions, a);
				}
			}
		}

		cairo_set_source_rgb(cr, 0.0, 0.53125, 0.26525);
		cairo_rectangle(cr, 0, 0, width-1, height-1);
		cairo_fill(cr);

		if (sem_action_list_execute(&actions, timer_ctx.now) != SEM_OK) {
			return sem_fatal_error();
		}

		cairo_set_line_width(cr, 0.1);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

		sem_render_world(&render_ctx, &world);

		char buf[128] = "";
		snprintf(buf, sizeof(buf), "frame: %ld, game time: %ld, multiplier: %f", frames, timer_ctx.now, timer_ctx.multiplier);
		cairo_move_to(cr, 0, 10);
		cairo_set_font_size(cr, 0.7);
		cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
		cairo_show_text(cr, buf);

		SDL_UnlockTexture(texture);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		frames++;
	}

	sem_dynamic_array_destroy(&actions);
	sem_world_destroy(&world);
	sem_train_destroy(&train1);
	sem_train_destroy(&train2);
	sem_train_destroy(&train3);
	sem_train_destroy(&train4);
	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}
