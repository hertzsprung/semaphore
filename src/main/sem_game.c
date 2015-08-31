#include "sem_game.h"
#include "sem_revenue.h"
#include "sem_world.h"

sem_success sem_game_init_blank(sem_game* game) {
	sem_revenue_init(&(game->revenue));
	return sem_world_init_blank(&(game->world));
}
