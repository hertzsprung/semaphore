#include <stdio.h>
#include <stdlib.h>

#include "sem_serialize.h"
#include "sem_error.h"
#include "sem_strings.h"

sem_success read_tiles(FILE* in, sem_world* world);
sem_success read_tile(FILE* in, sem_world* world);

sem_success sem_serialize_load(FILE* in, sem_world* world) {
	if (in == NULL) return sem_set_error("File does not exist");

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read line");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line);
	sem_tokenization_next(&tokens); // TODO: check token is "world"
	
	world->max_x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	world->max_y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	sem_world_init_blank(world);

	return read_tiles(in, world);
}

sem_success read_tiles(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read line");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line);
	char* token = sem_tokenization_next(&tokens);
	// TODO: check token is "tiles"
	
	uint32_t tiles = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	for (uint32_t i=0; i < tiles; i++) {
		sem_success success = read_tile(in, world);
		if (success != SEM_OK) return success;
	}
	
	token = sem_tokenization_next(&tokens);	

	return SEM_OK;
}

sem_success read_tile(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read line");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line);

	uint32_t x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	uint32_t y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	sem_tile* tile = sem_tile_at(world, x, y);
	tile->class = TRACK;

	return SEM_OK;
}
