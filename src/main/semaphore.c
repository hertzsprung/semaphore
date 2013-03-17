#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <cairo.h>

 static cairo_status_t stream_to(void* const closure, const unsigned char* const data, const unsigned int length); 

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return EXIT_FAILURE;
	}
	if (atexit(SDL_Quit) != 0) {
		SDL_Quit();
		return EXIT_FAILURE;
	}
	cairo_surface_t* const surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 128);
	/* TODO: check valid */
	cairo_t* const cr = cairo_create(surface);
	/* TODO: check with cairo_status() */
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 127, 127);
	cairo_stroke(cr);
	cairo_surface_write_to_png_stream(surface, stream_to, stdout);
	/* TODO: check == CAIRO_STATUS_SUCCESS */
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return EXIT_SUCCESS;
}

static cairo_status_t stream_to(void* const closure, const unsigned char* const data, const unsigned int length) {
	fwrite(data, sizeof(unsigned char), length, (FILE*) closure);
	/* TODO: check fwrite wrote all the data */
	return CAIRO_STATUS_SUCCESS;
}
