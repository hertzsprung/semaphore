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

int benchmark(void (*function)(), unsigned int iterations, struct timespec* delta);

void time_delta(struct timespec start, struct timespec end, struct timespec* delta);

long time_millis(struct timespec *ts);

void hello(void);

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

	//char buf[32] = "";

	/* random lines */
	for (int i = 0; i < 10000; i++) {
		cairo_set_source_rgb(cr, rand()/(double)RAND_MAX, rand()/(double)RAND_MAX, rand()/(double)RAND_MAX);
		cairo_set_line_width(cr, 5.0 * (rand()/(double)RAND_MAX));
		cairo_move_to(cr, rand() % (width+1), rand() % (height+1));
		cairo_line_to(cr, rand() % (width+1), rand() % (height+1));
		cairo_stroke(cr);
	}

	/* random circles */
	for (int i = 0; i < 10000; i++) {
		cairo_set_source_rgb(cr, rand()/(double)RAND_MAX, rand()/(double)RAND_MAX, rand()/(double)RAND_MAX);
		cairo_arc(cr, rand() % (width+1), rand() % (height+1), 32.0 * rand()/(double)RAND_MAX, 0, 2 * M_PI);
		cairo_fill(cr);
	}


	/*snprintf(buf, sizeof(buf), "%ldms",
		time_diff(time1, time2).tv_sec * 1000 +
		time_diff(time1, time2).tv_nsec / 1000000);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, 0, height);
	cairo_set_font_size(cr, 64);
	cairo_show_text(cr, buf);*/

	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(1000);

	struct timespec t;
	benchmark(&hello, 10, &t);
	printf("%ld", time_millis(&t));

	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}

/* TODO: pass a void* context along to the function, too */
int benchmark(void (*function)(), unsigned int iterations, struct timespec* delta) {
	struct timespec start, end;

	if (clock_gettime(CLOCK_REALTIME, &start) != 0) {
		return SEM_ERROR;
	}

	for (unsigned int i = 0; i < iterations; i++) {
		(*function)();
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

void hello(void) {
	printf("hello");
}
