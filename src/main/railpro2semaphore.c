#include <stdlib.h>
#include <stdio.h>

#include "sem_serialize.h"
#include "sem_serialize_railpro.h"
#include "sem_world.h"

int main() {
	sem_world world;
	if (sem_serialize_load_railpro(stdin, &world) != SEM_OK) return sem_fatal_error();
	if (sem_serialize_save(stdout, &world) != SEM_OK) return sem_fatal_error();
	sem_world_destroy(&world);
	return EXIT_SUCCESS;
}
