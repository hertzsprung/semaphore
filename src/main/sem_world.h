#ifndef _SEM_WORLD_H
#define _SEM_WORLD_H

#include "sem_train.h"

typedef enum {
	BLANK
} sem_tile_class;

typedef struct {
	sem_tile_class class;	
} sem_tile;

typedef struct {
	sem_train* train;
	uint32_t x_max;
	uint32_t y_max;
	sem_tile* tiles;
} sem_world;

#endif
