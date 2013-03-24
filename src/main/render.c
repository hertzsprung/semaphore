#include <stdio.h>
#include "render.h"
#include "graphics.h"
#include "world.h"

Status render_render(World* world, Graphics* const graphics) {
	world = NULL;
	graphics_move_to(graphics, 0, 0);
	graphics_line_to(graphics, 127, 127);
	graphics_stroke(graphics);
	return SUCCESS;
}
