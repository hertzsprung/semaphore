#include <stdio.h>
#include <stdlib.h>

#include "sem_serialize.h"
#include "sem_error.h"
#include "sem_strings.h"

sem_success sem_serialize_load(FILE* in, sem_world* world) {
	if (in == NULL) return sem_set_error("File does not exist");

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read line");
	sem_tokenization tokenization;
	sem_tokenization_init(&tokenization, line);
	char* token = sem_tokenization_next(&tokenization);
	// TODO: check token is "world"
	
	token = sem_tokenization_next(&tokenization);
	uint32_t max_x = (uint32_t) strtoul(token, NULL, 10);
	
	token = sem_tokenization_next(&tokenization);
	uint32_t max_y = (uint32_t) strtoul(token, NULL, 10);

	world->max_x = max_x;
	world->max_y = max_y;

	sem_world_init_blank(world);

	return SEM_OK;
}
