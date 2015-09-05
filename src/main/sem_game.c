#include <stdlib.h>

#include "sem_dynamic_array.h"
#include "sem_game.h"
#include "sem_revenue.h"
#include "sem_world.h"

sem_success sem_game_init_blank(sem_game* game) {
	sem_revenue_init(&(game->revenue));

	game->labels = malloc(sizeof(sem_dynamic_array));
	if (game->labels == NULL) return sem_set_error("Could not allocate memory for labels");
	if (sem_dynamic_array_init(game->labels) != SEM_OK) return SEM_ERROR;

	return sem_world_init_blank(&(game->world));
}

void sem_game_destroy(sem_game* game) {
	sem_world_destroy(&(game->world));
	for (uint32_t i=0; i < game->labels->tail_idx; i++) {
		free(((sem_label*) game->labels->items[i])->text);
		free(game->labels->items[i]);
	}
	sem_dynamic_array_destroy(game->labels);
}
