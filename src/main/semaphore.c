#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>

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
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, width, height);
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);

	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(5000);

	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}
