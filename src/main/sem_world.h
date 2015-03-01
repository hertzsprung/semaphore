#ifndef _SEM_WORLD_H
#define _SEM_WORLD_H

typedef struct sem_world sem_world;
typedef struct sem_track sem_track;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_train.h"

typedef enum {
	BLANK, TRACK, POINTS
} sem_tile_class;

struct sem_track {
	unit_vector start;
	unit_vector end;	
	sem_track* next; /**< when two tracks appear on the same tile, this points to the second piece of track */
};

typedef struct {
	sem_tile_class class;	
	sem_track* track;
	sem_track* points[3];		
} sem_tile;

struct sem_world {
	sem_dynamic_array* trains;
	uint32_t max_x;
	uint32_t max_y;
	sem_tile* tiles;
};

sem_success sem_world_init_blank(sem_world* world);

void sem_world_destroy(sem_world* world);

sem_success sem_world_add_train(sem_world* world, sem_train* train);

sem_tile* sem_tile_at_coord(sem_world* world, sem_coordinate* c);

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y);

/**
 * Redirect a train that has moved onto a new tile.
 *
 * @param relocated_track If the train is accepted onto the new tile, this points to the track which the head of the train now occupies, otherwise NULL.
 */
sem_success sem_tile_redirect(sem_train* train, sem_tile* tile, sem_track** relocated_track);

void sem_tile_switch_points(sem_tile* tile);

void sem_tile_set_track(sem_tile* tile, sem_track* track);

void sem_track_set(sem_track* track, unit_vector start, unit_vector end);

#endif
