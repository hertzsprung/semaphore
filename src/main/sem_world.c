#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

sem_success sem_track_accept(sem_train* train, sem_track* track, sem_tile_acceptance* acceptance);

sem_success sem_world_init_blank(sem_world* world) {
	world->trains = malloc(sizeof(sem_dynamic_array));
	if (world->trains == NULL) {
		return sem_set_error("Could not allocate memory for trains");
	}
	if (sem_dynamic_array_init(world->trains) != SEM_OK) return SEM_ERROR;

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
	sem_dynamic_array_destroy(world->trains);
	free(world->trains);
	free(world->tiles);
}

sem_success sem_world_add_train(sem_world* world, sem_train* train) {
	if (sem_dynamic_array_add(world->trains, train) != SEM_OK) return SEM_ERROR;
	train->world = world;

	return SEM_OK;
}

sem_tile* sem_tile_at_coord(sem_world* world, sem_coordinate* c) {
	return sem_tile_at(world, c->x, c->y);
}

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y) {
	assert(x < world->max_x);
	assert(y < world->max_y);

	return &(world->tiles[y*world->max_x + x]);
}

sem_success sem_tile_accept(sem_train* train, sem_tile* tile, sem_tile_acceptance* acceptance) {
	switch (tile->class) {
	case BLANK:
		return sem_set_error("Train ran onto blank tile");
	case TRACK:
		return sem_track_accept(train, tile->track, acceptance);
	case POINTS:
		return sem_track_accept(train, tile->track, acceptance);
		// TODO: here, we should look at the track in tile->points[] and see
		// if any of them are acceptable.  if one is, the train becomes derailed,
		// or we auto-switch the points if the train is at a slow speed
	}
}

void sem_tile_set_track(sem_tile* tile, sem_track* track) {
	tile->class = TRACK;
	tile->track = track;
}

void sem_track_set(sem_track* track, unit_vector start, unit_vector end) {
	track->start = start;
	track->end = end;
	track->next = NULL;
}

// private functions

sem_success sem_track_accept(sem_train* train, sem_track* track, sem_tile_acceptance* acceptance) {
	acceptance->track = NULL;
	bool accepted = false;
	sem_track* t = track;
	do {
		if (train->direction == sem_compass_opposite_of(t->start)) {
			// TODO: don't change the train direction in here
			// instead, pass in a ptr to a sem_tile_acceptance struct
			// that we mutate in here
			train->direction = t->end;
			accepted = true;
		} else if (train->direction == sem_compass_opposite_of(t->end)) {
			train->direction = t->start;
			accepted = true;
		}

		if (accepted) {
			acceptance->track = t;
		}
		t = t->next;	
	} while (!accepted && t != NULL);

	return accepted ? SEM_OK : sem_set_error("Train ran onto disconnected track");
}

void sem_tile_switch_points(sem_tile* tile) {
	for (uint8_t i=0; i<3; i++) {
		if (tile->points[i] == tile->track) {
			do {
				i = (i+1) % 3;
			} while (tile->points[i] == NULL);
			tile->track = tile->points[i];
			return;
		}
	}
	assert(false && "Track is not one of the points settings");
}
