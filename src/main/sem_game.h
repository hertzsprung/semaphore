#ifndef SEM_GAME_H
#define SEM_GAME_H

#include "sem_revenue.h"
#include "sem_world.h"

typedef struct {
	sem_world world;
	sem_revenue revenue;
} sem_game;

sem_success sem_game_init_blank(sem_game* game);

#endif
