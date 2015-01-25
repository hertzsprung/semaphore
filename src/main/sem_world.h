#ifndef _SEM_WORLD_H
#define _SEM_WORLD_H

#include "sem_error.h"
#include "sem_train.h"

typedef enum {
	BLANK
} sem_tile_class;

typedef struct {
	sem_tile_class class;	
} sem_tile;

typedef struct {
	sem_train* train;
	uint32_t max_x;
	uint32_t max_y;
	sem_tile* tiles;
} sem_world;

sem_success sem_world_init_blank(sem_world* world);

void sem_world_destroy(sem_world* world);

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y);

#endif
