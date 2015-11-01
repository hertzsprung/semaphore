#include <stdlib.h>
#include <stdio.h>

#include "sem_serialize.h"
#include "sem_serialize_railpro.h"
#include "sem_world.h"

int main() {
	sem_game game;
	if (sem_serialize_load_railpro(stdin, &game) != SEM_OK) return sem_fatal_error();
	if (sem_serialize_save(stdout, &game) != SEM_OK) return sem_fatal_error();
	sem_game_destroy(&game);
	return EXIT_SUCCESS;
}
