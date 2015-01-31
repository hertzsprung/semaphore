#include <assert.h>
#include <stdlib.h>

#include "sem_error.h"
#include "sem_world.h"

sem_success sem_track_redirect(sem_train* train, sem_track* track);

sem_success sem_world_init_blank(sem_world* world) {
	world->tiles = malloc(world->max_x * world->max_y * sizeof(sem_tile));
	if (world->tiles == NULL) {
		return sem_set_error("Could not allocate memory for tiles");
	}
	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			sem_tile_at(world, i, j)->class = BLANK;
		}
	}

	return SEM_OK;
}

void sem_world_destroy(sem_world* world) {
	free(world->tiles);
}

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y) {
	assert(x < world->max_x);
	assert(y < world->max_y);

	return &(world->tiles[y*world->max_x + x]);
}

sem_success sem_tile_redirect(sem_train* train, sem_tile* tile) {
	switch (tile->class) {
	case BLANK:
		return sem_set_error("Train ran onto blank tile");
	case TRACK:
		return sem_track_redirect(train, tile->track);
	}
}

sem_success sem_track_redirect(sem_train* train, sem_track* track) {
	if (train->direction == sem_compass_opposite_of(track->start)) {
		train->direction = track->end;
	} else if (train->direction == sem_compass_opposite_of(track->end)) {
		train->direction = track->start;
	} else {
		return sem_set_error("Train ran onto disconnected track");
	}
	return SEM_OK;
}
