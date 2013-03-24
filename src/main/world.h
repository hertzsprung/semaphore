#ifndef _SEMAPHORE_WORLD_H
#define _SEMAPHORE_WORLD_H

#include <stdint.h>

typedef struct World {
	uint8_t state;
} World;

World* world_initialise(void);

#endif
