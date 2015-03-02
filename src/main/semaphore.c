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


	sem_world world;
	world.max_x = 64;
	world.max_y = 64;
	sem_world_init_blank(&world);

	for (uint32_t i=0; i<=1; i++) {
		sem_tile* t = sem_tile_at(&world, i*8 + 1, 0);
		t->class = TRACK;
		sem_track track_SW_E;
		sem_track_set(&track_SW_E, SEM_SOUTH | SEM_WEST, SEM_EAST);
		t->track = &track_SW_E;

		t = sem_tile_at(&world, i*8 + 3, 0);
		t->class = TRACK;
		sem_track track_W_SE;
		sem_track_set(&track_W_SE, SEM_WEST, SEM_SOUTH | SEM_EAST);
		t->track = &track_W_SE;

		t = sem_tile_at(&world, i*8, 1);
		t->class = TRACK;
		sem_track track_S_NE;
		sem_track_set(&track_S_NE, SEM_SOUTH, SEM_NORTH | SEM_EAST);
		t->track = &track_S_NE;

		t = sem_tile_at(&world, i*8 + 4, 1);
		t->class = TRACK;
		sem_track track_S_NW;
		sem_track_set(&track_S_NW, SEM_SOUTH, SEM_NORTH | SEM_WEST);
		t->track = &track_S_NW;

		t = sem_tile_at(&world, i*8, 3);
		t->class = TRACK;
		sem_track track_N_SE;
		sem_track_set(&track_N_SE, SEM_NORTH, SEM_SOUTH | SEM_EAST);
		t->track = &track_N_SE;

		t = sem_tile_at(&world, i*8 + 4, 3);
		t->class = TRACK;
		sem_track track_N_SW;
		sem_track_set(&track_N_SW, SEM_NORTH, SEM_SOUTH | SEM_WEST);
		t->track = &track_N_SW;

		t = sem_tile_at(&world, i*8 + 1, 4);
		t->class = TRACK;
		sem_track track_NW_E;
		sem_track_set(&track_NW_E, SEM_NORTH | SEM_WEST, SEM_EAST);
		t->track = &track_NW_E;

		t = sem_tile_at(&world, i*8 + 3, 4);
		t->class = TRACK;
		sem_track track_W_NE;
		sem_track_set(&track_W_NE, SEM_WEST, SEM_NORTH | SEM_EAST);
		t->track = &track_W_NE;

		// straight track

		t = sem_tile_at(&world, i*8 + 2, 0);
		t->class = TRACK;
		sem_track trackEW;
		sem_track_set(&trackEW, SEM_EAST, SEM_WEST);
		t->track = &trackEW;

		t = sem_tile_at(&world, i*8 + 2, 4);
		t->class = TRACK;
		t->track = &trackEW;

		t = sem_tile_at(&world, i*8, 2);
		t->class = TRACK;
		sem_track trackNS;
		sem_track_set(&trackNS, SEM_NORTH, SEM_SOUTH);
		t->track = &trackNS;

		t = sem_tile_at(&world, i*8 + 4, 2);
		t->class = TRACK;
		t->track = &trackNS;
	}

	sem_track trackNE_SW;
	sem_track_set(&trackNE_SW, SEM_NORTH | SEM_WEST, SEM_SOUTH | SEM_EAST);

	sem_track trackNW_SE__NE_SW;
	sem_track_set(&trackNW_SE__NE_SW, SEM_NORTH | SEM_EAST, SEM_SOUTH | SEM_WEST);
	trackNW_SE__NE_SW.next = &trackNE_SW;

	sem_tile* t = sem_tile_at(&world, 6, 2);
	t->class = TRACK;
	t->track = &trackNW_SE__NE_SW;

	sem_track trackW_SE;
	sem_track_set(&trackW_SE, SEM_WEST, SEM_SOUTH | SEM_EAST);

	t = sem_tile_at(&world, 5, 1);
	t->class = TRACK;
	t->track = &trackW_SE;

	sem_track trackNW_E;
	sem_track_set(&trackNW_E, SEM_NORTH | SEM_WEST, SEM_EAST);

	t = sem_tile_at(&world, 7, 3);
	t->class = TRACK;
	t->track = &trackNW_E;

	sem_track trackW_NE;
	sem_track_set(&trackW_NE, SEM_WEST, SEM_NORTH | SEM_EAST);

	t = sem_tile_at(&world, 5, 3);
	t->class = TRACK;
	t->track = &trackW_NE;

	sem_track trackSW_E;
	sem_track_set(&trackSW_E, SEM_SOUTH | SEM_WEST, SEM_EAST);

	t = sem_tile_at(&world, 7, 1);
	t->class = TRACK;
	t->track = &trackSW_E;

	sem_track trackNW_S;
	sem_track_set(&trackNW_S, SEM_NORTH | SEM_WEST, SEM_SOUTH);

	t = sem_tile_at(&world, 4, 1);
	t->class = POINTS;
	t->track = &trackNW_S;
	t->points[0] = &trackNW_S;
	t->points[1] = &trackNW_E;
	t->points[2] = NULL;

	sem_track trackN_SE;
	sem_track_set(&trackN_SE, SEM_NORTH, SEM_SOUTH | SEM_EAST);

	t = sem_tile_at(&world, 8, 3);
	t->class = POINTS;
	t->track = &trackN_SE;
	t->points[0] = &trackN_SE;
	t->points[1] = &trackW_SE;
	t->points[2] = NULL;

	sem_track trackSW_N;
	sem_track_set(&trackSW_N, SEM_SOUTH | SEM_WEST, SEM_NORTH);

	t = sem_tile_at(&world, 4, 3);
	t->class = POINTS;
	t->track = &trackSW_N;
	t->points[0] = &trackSW_N;
	t->points[1] = &trackSW_E;
	t->points[2] = NULL;

	sem_track trackNE_S;
	sem_track_set(&trackNE_S, SEM_NORTH | SEM_EAST, SEM_SOUTH);

	t = sem_tile_at(&world, 8, 1);
	t->class = POINTS;
	t->track = &trackNE_S;
	t->points[0] = &trackNE_S;
	t->points[1] = &trackW_NE;
	t->points[2] = NULL;

	// straight collision track

	sem_track trackEW;
	sem_track_set(&trackEW, SEM_EAST, SEM_WEST);

	for (uint32_t i=0; i<16; i++) {
		sem_tile* ts = sem_tile_at(&world, i, 8);
		ts->class = TRACK;
		ts->track = &trackEW;
	}

	// train 1
	sem_train train1;
	sem_train_init(&train1);
	sem_coordinate train1Car1Position;
	sem_coordinate_set(&train1Car1Position, 2, 0);
	sem_car train1Car1;
	train1Car1.position = &train1Car1Position;
	train1Car1.track = sem_tile_at_coord(&world, &train1Car1Position)->track;
	sem_train_add_car(&train1, &train1Car1);

	sem_coordinate train1Car2Position;
	sem_coordinate_set(&train1Car2Position, 1, 0);
	sem_car train1Car2;
	train1Car2.position = &train1Car2Position;
	train1Car2.track = sem_tile_at_coord(&world, &train1Car2Position)->track;
	sem_train_add_car(&train1, &train1Car2);

	sem_coordinate train1Car3Position;
	sem_coordinate_set(&train1Car3Position, 0, 1);
	sem_car train1Car3;
	train1Car3.position = &train1Car3Position;
	train1Car3.track = sem_tile_at_coord(&world, &train1Car3Position)->track;
	sem_train_add_car(&train1, &train1Car3);

	train1.direction = SEM_EAST;

	// train 2
	sem_train train2;
	sem_train_init(&train2);
	sem_coordinate train2Car1Position;
	sem_coordinate_set(&train2Car1Position, 10, 0);
	sem_car train2Car1;
	train2Car1.position = &train2Car1Position;
	train2Car1.track = sem_tile_at_coord(&world, &train2Car1Position)->track;
	sem_train_add_car(&train2, &train2Car1);

	sem_coordinate train2Car2Position;
	sem_coordinate_set(&train2Car2Position, 11, 0);
	sem_car train2Car2;
	train2Car2.position = &train2Car2Position;
	train2Car2.track = sem_tile_at_coord(&world, &train2Car2Position)->track;
	sem_train_add_car(&train2, &train2Car2);

	sem_coordinate train2Car3Position;
	sem_coordinate_set(&train2Car3Position, 12, 1);
	sem_car train2Car3;
	train2Car3.position = &train2Car3Position;
	train2Car3.track = sem_tile_at_coord(&world, &train2Car3Position)->track;
	sem_train_add_car(&train2, &train2Car3);

	train2.direction = SEM_WEST;

	// train 3
	sem_train train3;
	sem_train_init(&train3);
	sem_coordinate train3Car1Position;
	sem_coordinate_set(&train3Car1Position, 2, 8);
	sem_car train3Car1;
	train3Car1.position = &train3Car1Position;
	train3Car1.track = sem_tile_at_coord(&world, &train3Car1Position)->track;
	sem_train_add_car(&train3, &train3Car1);

	sem_coordinate train3Car2Position;
	sem_coordinate_set(&train3Car2Position, 1, 8);
	sem_car train3Car2;
	train3Car2.position = &train3Car2Position;
	train3Car2.track = sem_tile_at_coord(&world, &train3Car2Position)->track;
	sem_train_add_car(&train3, &train3Car2);

	sem_coordinate train3Car3Position;
	sem_coordinate_set(&train3Car3Position, 0, 8);
	sem_car train3Car3;
	train3Car3.position = &train3Car3Position;
	train3Car3.track = sem_tile_at_coord(&world, &train3Car3Position)->track;
	sem_train_add_car(&train3, &train3Car3);

	train3.direction = SEM_EAST;

	// train 4
	sem_train train4;
	sem_train_init(&train4);
	train4.state = DERAILED;

	sem_coordinate train4Car1Position;
	sem_coordinate_set(&train4Car1Position, 13, 8);
	sem_car train4Car1;
	train4Car1.position = &train4Car1Position;
	train4Car1.track = sem_tile_at_coord(&world, &train4Car1Position)->track;
	sem_train_add_car(&train4, &train4Car1);

	sem_coordinate train4Car2Position;
	sem_coordinate_set(&train4Car2Position, 14, 8);
	sem_car train4Car2;
	train4Car2.position = &train4Car2Position;
	train4Car2.track = sem_tile_at_coord(&world, &train4Car2Position)->track;
	sem_train_add_car(&train4, &train4Car2);

	sem_coordinate train4Car3Position;
	sem_coordinate_set(&train4Car3Position, 15, 8);
	sem_car train4Car3;
	train4Car3.position = &train4Car3Position;
	train4Car3.track = sem_tile_at_coord(&world, &train4Car3Position)->track;
	sem_train_add_car(&train4, &train4Car3);

	train4.direction = SEM_WEST;

	sem_world_add_train(&world, &train1);
	sem_world_add_train(&world, &train2);
	sem_world_add_train(&world, &train3);
	sem_world_add_train(&world, &train4);

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
