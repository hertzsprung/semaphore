#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cairo.h>

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#define SEM_OK 0
#define SEM_ERROR -1

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
		fprintf(stderr,
			"Unable to initialize SDL: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}
	atexit(SDL_Quit);

	SDL_Window* window;
	SDL_Renderer* renderer;
	if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) != 0) {
		fprintf(stderr,
			"Unable to create window: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	SDL_Texture *texture;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);
	if (texture == 0) {
		fprintf(stderr,
			"Unable to create texture: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	void *pixels;
	int pitch;

	if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
		fprintf(stderr,
			"Failed to lock texture: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
		pixels,
		CAIRO_FORMAT_ARGB32,
		width, height, pitch);

	cairo_t *cr = cairo_create(cairo_surface);

	char buf[32] = "";

	struct timespec t;
	struct benchmark_cairo_line_context ctx;
	ctx.cr = cr;
	ctx.width = width;
	ctx.height = height;
	benchmark(&benchmark_cairo_line, &ctx, 10000, &t);
	snprintf(buf, sizeof(buf), "%ldms", time_millis(&t));

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_rectangle(cr, 0, height-64, width, 64);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_move_to(cr, 0, height);
	cairo_set_font_size(cr, 64);
	cairo_show_text(cr, buf);

	benchmark(&benchmark_cairo_circle, &ctx, 10000, &t);

	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(3000);

	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}

int benchmark(int (*function)(void*), void* context, unsigned int iterations, struct timespec* delta) {
	struct timespec start, end;

	if (clock_gettime(CLOCK_REALTIME, &start) != 0) {
		return SEM_ERROR;
	}

	for (unsigned int i = 0; i < iterations; i++) {
		if ((*function)(context) != 0) {
			return SEM_ERROR;
		}
	}

	if (clock_gettime(CLOCK_REALTIME, &end) != 0) {
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
	cairo_move_to(ctx->cr, rand() % (ctx->width+1), rand() % (ctx->height+1));
	cairo_line_to(ctx->cr, rand() % (ctx->width+1), rand() % (ctx->height+1));
	cairo_stroke(ctx->cr);

	return SEM_OK;
}

int benchmark_cairo_circle(void* void_ctx) {
	struct benchmark_cairo_line_context* ctx = (struct benchmark_cairo_line_context*) void_ctx;

	cairo_set_source_rgb(ctx->cr, rand_01(), rand_01(), rand_01());
	cairo_arc(ctx->cr, rand() % (ctx->width+1), rand() % (ctx->height+1), 32.0 * rand_01(), 0, 2 * M_PI);
	cairo_fill(ctx->cr);

	return SEM_OK;
}
