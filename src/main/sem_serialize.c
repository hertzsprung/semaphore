#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_serialize.h"
#include "sem_error.h"
#include "sem_parser.h"
#include "sem_strings.h"

sem_success read_dimensions(FILE* in, sem_world* world);
sem_success read_now(FILE* in, sem_world* world);
sem_success read_multiplier(FILE* in, sem_world* world);
sem_success read_tiles(FILE* in, sem_world* world);
sem_success read_tile(FILE* in, sem_world* world);
sem_success read_trains(FILE* in, sem_world* world);
sem_success read_train(FILE* in, sem_world* world);
sem_success read_train_state(FILE* in, sem_train* train);
sem_success read_train_direction(FILE* in, sem_train* train);
sem_success read_train_cars(FILE* in, sem_train* train);
sem_success read_car(FILE* in, sem_train* train);

sem_success write_timer(FILE* out, sem_world* world);
sem_success write_tiles(FILE* out, sem_world* world);
sem_success write_tile(FILE* out, uint32_t x, uint32_t y, sem_tile* tile);

sem_success sem_serialize_load(FILE* in, sem_world* world) {
	if (in == NULL) return sem_set_error("File does not exist");

	if (read_dimensions(in, world) != SEM_OK) return SEM_ERROR;
	if (read_now(in, world) != SEM_OK) return SEM_ERROR;
	if (read_multiplier(in, world) != SEM_OK) return SEM_ERROR;
	if (read_tiles(in, world) != SEM_OK) return SEM_ERROR;
	if (read_trains(in, world) != SEM_OK) return SEM_ERROR;
	return SEM_OK;
}

sem_success sem_serialize_save(FILE* out, sem_world* world) {
	fprintf(out, "world %d %d\n", world->max_x, world->max_y); // TODO: check status of fprintf
	if (write_timer(out, world) != SEM_OK) return SEM_ERROR;
	if (write_tiles(out, world) != SEM_OK) return SEM_ERROR;
	fprintf(out, "trains 0\n");

	return SEM_OK;
}

sem_success read_dimensions(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read world dimensions");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "world"
	
	world->max_x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	world->max_y = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	free(line);

	return sem_world_init_blank(world);
}

sem_success read_now(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read current time");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "now"

	world->timer->now = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	free(line);

	return SEM_OK;
}

sem_success read_multiplier(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read timer multiplier");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "now"

	world->timer->multiplier = strtod(sem_tokenization_next(&tokens), NULL);

	free(line);

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
	sem_train_init(train);
	train->world = world;

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train name");
	// TODO: check first token is "train", second token is train name
	free(line);

	if (read_train_state(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_direction(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_cars(in, train) != SEM_OK) return SEM_ERROR;

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

sem_success read_train_direction(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train direction");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "direction"

	if (sem_parse_unit_vector(&(train->direction), sem_tokenization_next(&tokens)) != SEM_OK) return SEM_ERROR;
	
	free(line);

	return SEM_OK;
}

sem_success read_train_cars(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read cars count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "cars"
	
	uint32_t cars = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	for (uint32_t i=0; i < cars; i++) {
		if (read_car(in, train) != SEM_OK) return SEM_ERROR;
	}

	free(line);

	return SEM_OK;
}

sem_success read_car(FILE* in, sem_train* train) {
	sem_car* car = malloc(sizeof(sem_car));
	if (car == NULL) return sem_set_error("Failed to allocated memory for car");

	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read car");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");

	uint32_t x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	uint32_t y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	car->position.x = x;
	car->position.y = y;

	sem_tile* tile = sem_tile_at(train->world, x, y);
	sem_track key;
	sem_parse_track_part(&key, sem_tokenization_next(&tokens));
	if (sem_track_matching(tile, &key, &(car->track)) != SEM_OK) return SEM_ERROR;

	free(line);

	return sem_train_add_car(train, car);
}

sem_success write_timer(FILE* out, sem_world* world) {
	fprintf(out, "now %lu\nmultiplier %lf\n", world->timer->now, world->timer->multiplier);

	return SEM_OK;
}

sem_success write_tiles(FILE* out, sem_world* world) {
	uint32_t tiles = 0;
	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			if (sem_tile_at(world, i, j)->class != BLANK) {
				tiles++;
			}
		}
	}

	fprintf(out, "tiles %d\n", tiles); // TODO: check return

	for (uint32_t j=0; j<world->max_y; j++) {
		for (uint32_t i=0; i<world->max_x; i++) {
			sem_tile* tile = sem_tile_at(world, i, j);
			if (tile->class != BLANK) {
				write_tile(out, i, j, tile);
			}
		}
	}

	return SEM_OK;
}

sem_success write_tile(FILE* out, uint32_t x, uint32_t y, sem_tile* tile) {
	fprintf(out, "%d %d ", x, y);
	sem_tile_print(out, tile);
	fprintf(out, "\n");

	return SEM_OK;
}
