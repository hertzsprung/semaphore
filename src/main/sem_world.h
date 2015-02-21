#ifndef _SEM_WORLD_H
#define _SEM_WORLD_H

typedef struct sem_world sem_world;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_train.h"

typedef enum {
	BLANK, TRACK
} sem_tile_class;

typedef struct {
	unit_vector start;
	unit_vector end;	
} sem_track;

typedef struct {
	sem_tile_class class;	
	sem_track* track;
} sem_tile;

struct sem_world {
	sem_dynamic_array* trains;
	uint32_t max_x;
	uint32_t max_y;
	sem_tile* tiles;
};

sem_success sem_world_init_blank(sem_world* world);

void sem_world_destroy(sem_world* world);

sem_tile* sem_tile_at_coord(sem_world* world, sem_coordinate* c);

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y);

sem_success sem_tile_redirect(sem_train* train, sem_tile* tile);

#endif
