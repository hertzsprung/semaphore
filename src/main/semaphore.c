#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cairo.h>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "sem_error.h"
#include "sem_render.h"
#include "sem_train.h"

int benchmark(int (*function)(void*), void* context, unsigned int iterations, struct timespec* delta);

void time_delta(struct timespec start, struct timespec end, struct timespec* delta);

long time_millis(struct timespec *ts);

int benchmark_cairo_line(void* context);

int benchmark_cairo_circle(void* context);

double rand_01(void);

struct benchmark_cairo_line_context {
	cairo_t *cr;
	int width, height;
};


int main(/*int argc, char **argv*/) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sem_set_error("Unable to initialize SDL: %s", SDL_GetError());
		return sem_fatal_error();
	}
	atexit(SDL_Quit);

	SDL_Window* window;
	SDL_Renderer* renderer;
	if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) != 0) {
		sem_set_error("Unable to create window: %s", SDL_GetError());
		return sem_fatal_error();
	}

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	SDL_Texture *texture;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);
	if (texture == 0) {
		sem_set_error("Unable to create texture: %s", SDL_GetError());
		return sem_fatal_error();
	}

	SDL_Event e;
	bool quit = false;	
	while (!quit) {
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				quit = true;
			}
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
		cairo_scale(cr, 32.0, 32.0);
		cairo_set_line_width(cr, 0.1);

		sem_render_context render_ctx;
		render_ctx.cr = cr;	

		sem_train train;
		train.x = 0;
		train.y = 0;

		sem_render_train(&render_ctx, &train);

		SDL_UnlockTexture(texture);

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		cairo_destroy(cr);
	}

	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}

int benchmark(int (*function)(void*), void* context, unsigned int iterations, struct timespec* delta) {
	struct timespec start, end;

	if (clock_gettime(CLOCK_REALTIME, &start) != 0) {
		sem_set_error("Could not get system clock");
		return SEM_ERROR;
	}

	for (unsigned int i = 0; i < iterations; i++) {
		if ((*function)(context) != 0) {
			return SEM_ERROR;
		}
	}

	if (clock_gettime(CLOCK_REALTIME, &end) != 0) {
		sem_set_error("Could not get system clock");
		return SEM_ERROR;
	}

	time_delta(start, end, delta);

	return SEM_OK;
}

void time_delta(struct timespec start, struct timespec end, struct timespec* delta) {
	if (end.tv_nsec - start.tv_nsec < 0) {
		delta->tv_sec = end.tv_sec - start.tv_sec - 1;
		delta->tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		delta->tv_sec = end.tv_sec - start.tv_sec;
		delta->tv_nsec = end.tv_nsec - start.tv_nsec;
	}
}

long time_millis(struct timespec *ts) {
	return ts->tv_sec*1000 + ts->tv_nsec/1000000;
}

double rand_01(void) {
	return rand() / (double)RAND_MAX;
}

int benchmark_cairo_line(void* void_ctx) {
	struct benchmark_cairo_line_context* ctx = (struct benchmark_cairo_line_context*) void_ctx;

	cairo_set_source_rgb(ctx->cr, rand_01(), rand_01(), rand_01());
	cairo_set_line_width(ctx->cr, 5.0 * rand_01());
	cairo_move_to(ctx->cr, rand() % ctx->width, rand() % ctx->height);
	cairo_line_to(ctx->cr, rand() % ctx->width, rand() % ctx->height);
	cairo_stroke(ctx->cr);

	return SEM_OK;
}

int benchmark_cairo_circle(void* void_ctx) {
	struct benchmark_cairo_line_context* ctx = (struct benchmark_cairo_line_context*) void_ctx;

	cairo_set_source_rgb(ctx->cr, rand_01(), rand_01(), rand_01());
	cairo_arc(ctx->cr, rand() % ctx->width, rand() % ctx->height, 32.0 * rand_01(), 0, 2 * M_PI);
	cairo_fill(ctx->cr);

	return SEM_OK;
}
