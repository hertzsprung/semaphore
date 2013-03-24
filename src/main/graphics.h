#ifndef _SEMAPHORE_GRAPHICS_H
#define _SEMAPHORE_GRAPHICS_H

#include <cairo.h>
#include "types.h"

typedef struct Graphics {
	cairo_surface_t* surface;
	cairo_t* cr;
} Graphics;

Graphics* graphics_initialise(void);
Status graphics_screenshot(Graphics* graphics, const char* filename);
void graphics_destroy(Graphics* graphics);

#endif
