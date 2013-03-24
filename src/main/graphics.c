#include <stdlib.h>
#include <cairo.h>
#include "graphics.h"

Graphics* graphics_initialise(void) {
	Graphics* graphics = malloc(sizeof(Graphics));
	graphics->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 128);
	/* TODO: check valid */
	graphics->cr = cairo_create(graphics->surface);
	/* TODO: check with cairo_status() */
	return graphics;
}

void graphics_move_to(Graphics* graphics, double x, double y) {
	cairo_move_to(graphics->cr, x, y);
}

void graphics_line_to(Graphics* graphics, double x, double y) {
	cairo_line_to(graphics->cr, x, y);
}

void graphics_stroke(Graphics* graphics) {
	cairo_stroke(graphics->cr);
}

Status graphics_screenshot(Graphics* graphics, const char* const filename) {
	if (cairo_surface_write_to_png(graphics->surface, filename) == CAIRO_STATUS_SUCCESS) {
		return SUCCESS;
	} else {
		/* TODO: log */
		return FAILURE;
	}
}

void graphics_destroy(Graphics* graphics) {
	cairo_destroy(graphics->cr);
	cairo_surface_destroy(graphics->surface);
	free(graphics);
}
