#ifndef SEM_GAME_H
#define SEM_GAME_H

#include "sem_dynamic_array.h"
#include "sem_revenue.h"
#include "sem_world.h"

typedef struct {
	sem_world world;
	sem_revenue revenue;
	sem_dynamic_array* labels;
} sem_game;

sem_success sem_game_init_blank(sem_game* game);

void sem_game_destroy(sem_game* game);

#endif
