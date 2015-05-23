#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>

#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_track_cache.h"
#include "sem_timer.h"
#include "sem_world.h"

sem_success sem_track_accept(sem_train* train, sem_track* track, sem_tile_acceptance* acceptance);
sem_success sem_inactive_track_accept(sem_train* train, sem_tile* tile, sem_tile_acceptance* acceptance);
void sem_destroy_signals(sem_world* world);
void track_matching(sem_track* track, sem_track* key, sem_track** match);

sem_success sem_world_init_blank(sem_world* world) {
	world->timer = malloc(sizeof(sem_timer_context));
	if (world->timer == NULL) return sem_set_error("Could not allocate memory for timer");
	if (sem_timer_init_default(world->timer) != SEM_OK) return SEM_ERROR;

	world->trains = malloc(sizeof(sem_dynamic_array));
	if (world->trains == NULL) return sem_set_error("Could not allocate memory for trains");
	if (sem_dynamic_array_init(world->trains) != SEM_OK) return SEM_ERROR;

	world->actions = malloc(sizeof(sem_dynamic_array));
	if (world->actions == NULL) return sem_set_error("Could not allocate memory for actions");
	if (sem_heap_init(world->actions) != SEM_OK) return SEM_ERROR;

	world->track_cache = malloc(sizeof(sem_track_cache));
	if (world->track_cache == NULL) return sem_set_error("Could not allocate memory for track cache");
	if (sem_track_cache_init(world->track_cache) != SEM_OK) return SEM_ERROR;

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
	for (uint32_t i=0; i<world->trains->tail_idx; i++) {
		sem_train_destroy(world->trains->items[i]);
		free(world->trains->items[i]);
	}
	sem_dynamic_array_destroy(world->trains);
	sem_dynamic_array_destroy(world->actions); // TODO: we might need to free() malloc'd actions within this list before deleting the action list itself
	sem_destroy_signals(world);
	sem_track_cache_destroy(world->track_cache);
	free(world->trains);
	free(world->timer);
	free(world->tiles);
	free(world->track_cache);
}

void sem_destroy_signals(sem_world* world) {
	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			sem_tile* tile = sem_tile_at(world, i, j);
			if (tile->class == SIGNAL) free(tile->signal);
		}
	}
}

sem_success sem_world_add_train(sem_world* world, sem_train* train) {
	if (sem_dynamic_array_add(world->trains, train) != SEM_OK) return SEM_ERROR;
	train->world = world;

	return SEM_OK;
}

sem_train* sem_train_by_id(sem_world* world, uuid_t id) {
	for (uint32_t i=0; i<world->trains->tail_idx; i++) {
		sem_train* train = world->trains->items[i];
		if (uuid_compare(train->id, id) == 0) return train;
	}

	return NULL;
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
	sem_tile_acceptance_init(acceptance);

	switch (tile->class) {
	case BLANK:
		return sem_set_error("Train ran onto blank tile");
	case TRACK:
	case SIGNAL: // TODO: signalling logic
		return sem_track_accept(train, tile->track, acceptance);
	case BUFFER:
		if (sem_track_accept(train, tile->track, acceptance) != SEM_OK) return SEM_ERROR;
		acceptance->reached_buffer = true;
		return SEM_OK;
	case POINTS:
		if (sem_track_accept(train, tile->track, acceptance) == SEM_OK) return SEM_OK;
		acceptance->need_points_switch = true;
		return sem_inactive_track_accept(train, tile, acceptance);
	}
}

void sem_tile_acceptance_init(sem_tile_acceptance* acceptance) {
	acceptance->direction = 0;
	acceptance->track = NULL;
	acceptance->need_points_switch = false;
	acceptance->reached_buffer = false;
}

void sem_tile_set_track(sem_tile* tile, sem_track* track) {
	tile->class = TRACK;
	tile->track = track;
}

void sem_tile_set_buffer(sem_tile* tile, sem_track* track) {
	tile->class = BUFFER;
	tile->track = track;
}

void sem_tile_set_points(sem_tile* tile, sem_track* track) {
	tile->class = POINTS;
	tile->track = track;
	tile->points[0] = track;
	tile->points[1] = NULL;
	tile->points[2] = NULL;
}

void sem_tile_set_signal(sem_tile* tile, sem_track* track, sem_signal* signal) {
	tile->class = SIGNAL;
	tile->track = track;
	tile->signal = signal;
}

void sem_track_set(sem_track* track, unit_vector start, unit_vector end) {
	track->start = start;
	track->end = end;
	track->next = NULL;
}

sem_success sem_tile_track_matching(sem_tile* tile, sem_track* key, sem_track** match) {
	*match = NULL;
	if (tile->class == POINTS) {
		for (uint8_t i=0; i<3; i++) {
			if (tile->points[i] == NULL) break;
			track_matching(tile->points[i], key, match);
			if (*match != NULL) return SEM_OK;
		}
	} else {
		track_matching(tile->track, key, match);
		if (*match != NULL) return SEM_OK;
	}

	return sem_set_error("Could not find desired track part in tile");
}

void track_matching(sem_track* track, sem_track* key, sem_track** match) {
	sem_track* t = track;
	do {
		if ((key->start == t->start && key->end == t->end) ||
		    (key->start == t->end && key->end == t->start)) {
			*match = t;
			return;
		}
		t = t->next;
	} while (t != NULL);
}

// private functions

sem_success sem_track_accept(sem_train* train, sem_track* track, sem_tile_acceptance* acceptance) {
	bool accepted = false;
	sem_track* t = track;
	do {
		if (train->direction == sem_compass_opposite_of(t->start)) {
			acceptance->direction = t->end;
			accepted = true;
		} else if (train->direction == sem_compass_opposite_of(t->end)) {
			acceptance->direction = t->start;
			accepted = true;
		}

		if (accepted) {
			acceptance->track = t;
		}
		t = t->next;	
	} while (!accepted && t != NULL);

	return accepted ? SEM_OK : sem_set_error("Train ran onto disconnected track");
}

sem_success sem_inactive_track_accept(sem_train* train, sem_tile* tile, sem_tile_acceptance* acceptance) {
	for (uint8_t i=0; i<3 && tile->points[i] != NULL; i++) {
		if (tile->points[i] == tile->track) continue;
		if (sem_track_accept(train, tile->points[i], acceptance) == SEM_OK) return SEM_OK;
	}

	return sem_set_error("Train ran onto disconnected track");
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

bool sem_track_active(sem_tile* tile, sem_track* track) {
	return tile->track == track;
}

bool sem_track_straight(sem_track* track) {
	return sem_compass_straight(track->start, track->end);
}

unit_vector sem_track_corner(sem_track* track) {
	return sem_compass_corner_of_curve(track->start, track->end);
}
