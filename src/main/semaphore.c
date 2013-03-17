#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <cairo.h>

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

	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		if (strstr(line, "SCREENSHOT") == line) {
			char* filename = strchr(line, ' ') + 1;
			filename[strlen(filename)-1] = '\0'; // nukes trailing newline
			cairo_surface_write_to_png(surface, filename);
			/* TODO: check == CAIRO_STATUS_SUCCESS */
		}
	}

	free(line); /* TODO: are we not leaking lines here? */
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return EXIT_SUCCESS;
}
