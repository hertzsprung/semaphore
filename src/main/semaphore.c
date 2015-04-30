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
#include "sem_serialize.h"
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

	FILE* map = fopen("maps/64x64test", "r");
	sem_world world;
	if (sem_serialize_load(map, &world) != SEM_OK) return sem_fatal_error();
	fclose(map);

	cairo_scale(cr, render_ctx.scale, render_ctx.scale);

	SDL_Event e;
	bool quit = false;	
	uint64_t frames = 0;
	while (!quit) {
		sem_timer_now(world.timer);

		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_LCTRL || e.key.keysym.sym == SDLK_RCTRL) {
					// ignore
				} else if (e.key.keysym.sym == SDLK_l) {
					sem_world_destroy(&world);
					FILE* load = fopen("build/main/saved_map", "r");
					if (sem_serialize_load(load, &world) != SEM_OK) return sem_fatal_error();
					fclose(load);
				} else if (e.key.keysym.sym == SDLK_s) {
					FILE* save = fopen("build/main/saved_map", "w");
					if (sem_serialize_save(save, &world) != SEM_OK) return sem_fatal_error();
					fclose(save);
				} else {
					quit = true;
				}
			}
			if (e.type == SDL_MOUSEWHEEL) {
				if (SDL_GetModState() & KMOD_CTRL) {
					if (e.wheel.y > 0) {
						world.timer->multiplier *= 1.1 * e.wheel.y;
					} else if (e.wheel.y < 0) {
						world.timer->multiplier /= -1.1 * e.wheel.y;
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
				input.time = world.timer->now;
				input.tile = &coord;

				sem_action* a = NULL;

				// TODO: wrap up train/tile input handlers into one big input routine
				sem_train_input_act_upon(&input, &world, &a);

				if (a != NULL) {
					// TODO: should just chuck this onto the action list with "immediate" flag set
					a->function(world.actions, a);
				} else {
					sem_tile_input_act_upon(&input, &world, &a);
					if (a != NULL) a->function(world.actions, a);
				}
			}
		}

		cairo_set_source_rgb(cr, 0.0, 0.53125, 0.26525);
		cairo_rectangle(cr, 0, 0, width-1, height-1);
		cairo_fill(cr);

		if (sem_action_list_execute(world.actions, world.timer->now) != SEM_OK) {
			return sem_fatal_error();
		}

		cairo_set_line_width(cr, 0.1);

		sem_render_world(&render_ctx, &world);

		char buf[128] = "";
		snprintf(buf, sizeof(buf), "frame: %ld, game time: %ld, multiplier: %f", frames, world.timer->now, world.timer->multiplier);
		cairo_move_to(cr, 0, 14);
		cairo_set_font_size(cr, 0.7);
		cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
		cairo_show_text(cr, buf);

		SDL_UnlockTexture(texture);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		frames++;
	}

	sem_world_destroy(&world);
	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}
