#include <stdlib.h>
#include "world.h"

World* world_initialise(void) {
	World* world = malloc(sizeof(World));
	return world;
}
