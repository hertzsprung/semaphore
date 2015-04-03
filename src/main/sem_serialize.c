#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_serialize.h"
#include "sem_error.h"
#include "sem_parser.h"
#include "sem_strings.h"

sem_success read_tiles(FILE* in, sem_world* world);
sem_success read_tile(FILE* in, sem_world* world);
sem_success read_trains(FILE* in, sem_world* world);
sem_success read_train(FILE* in, sem_world* world);
sem_success read_train_state(FILE* in, sem_train* train);

sem_success sem_serialize_load(FILE* in, sem_world* world) {
	if (in == NULL) return sem_set_error("File does not exist");

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read line");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "world"
	
	world->max_x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	world->max_y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	sem_world_init_blank(world);

	free(line);

	if (read_tiles(in, world) != SEM_OK) return SEM_ERROR;
	if (read_trains(in, world) != SEM_OK) return SEM_ERROR;
	return SEM_OK;
}

sem_success read_tiles(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read tiles count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	char* token = sem_tokenization_next(&tokens);
	// TODO: check token is "tiles"
	
	uint32_t tiles = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	for (uint32_t i=0; i < tiles; i++) {
		sem_success success = read_tile(in, world);
		if (success != SEM_OK) return success;
	}
	
	token = sem_tokenization_next(&tokens);	

	free(line);

	return SEM_OK;
}

sem_success read_tile(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read tile");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");

	uint32_t x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	uint32_t y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	sem_tile* tile = sem_tile_at(world, x, y);
	tile->class = TRACK;
	sem_tile_parse(tile, &tokens, world->track_cache);

	free(line);

	return SEM_OK;
}

sem_success read_trains(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read trains count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "trains"
	
	uint32_t trains = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	for (uint32_t i=0; i < trains; i++) {
		sem_success success = read_train(in, world);
		if (success != SEM_OK) return success;
	}

	free(line);

	return SEM_OK;
}

sem_success read_train(FILE* in, sem_world* world) {
	sem_train* train = malloc(sizeof(sem_train));
	if (train == NULL) return sem_set_error("Failed to allocated memory for train");

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train name");
	// TODO: check first token is "train", second token is train name
	free(line);

	if (read_train_state(in, train) != SEM_OK) return SEM_ERROR;

	line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train direction");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "direction"

	if (sem_parse_unit_vector(&(train->direction), sem_tokenization_next(&tokens)) != SEM_OK) return SEM_ERROR;
	
	free(line);

	sem_dynamic_array_add(world->trains, train); // TODO: free() this in sem_world_destroy()

	return SEM_OK;
}

sem_success read_train_state(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train state");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "state"

	char* state = sem_tokenization_next(&tokens);
	if (strcmp(state, "STOPPED") == 0) {
		train->state = STOPPED;
	}
	// TODO: other states

	free(line);

	return SEM_OK;
}
