#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_serialize.h"
#include "sem_serialize_actions.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_parser.h"
#include "sem_strings.h"

sem_success read_dimensions(FILE* in, sem_world* world);
sem_success read_now(FILE* in, sem_world* world);
sem_success read_multiplier(FILE* in, sem_world* world);
sem_success read_tiles(FILE* in, sem_world* world);
sem_success read_tile(FILE* in, sem_world* world);
sem_success read_trains(FILE* in, sem_world* world);
sem_success read_train(FILE* in, sem_world* world);
sem_success read_train_id(FILE* in, sem_train* train);
sem_success read_train_name(FILE* in, sem_train* train);
sem_success read_train_state(FILE* in, sem_train* train);
sem_success read_train_direction(FILE* in, sem_train* train);
sem_success read_train_cars(FILE* in, sem_train* train);
sem_success read_car(FILE* in, sem_train* train);
sem_success read_actions(FILE* in, sem_world* world);
sem_success read_action(FILE* in, sem_world* world);

sem_success write_timer(FILE* out, sem_world* world);
sem_success write_tiles(FILE* out, sem_world* world);
sem_success write_tile(FILE* out, uint32_t x, uint32_t y, sem_tile* tile);
sem_success write_trains(FILE* out, sem_dynamic_array* trains);
sem_success write_train(FILE* out, sem_train* train);
sem_success write_train_state(FILE* out, sem_train_state state);
sem_success write_car(FILE* out, sem_car* car);
sem_success write_actions(FILE* out, sem_dynamic_array* actions);
sem_success write_action(FILE* out, sem_action* action);

sem_success sem_serialize_load(FILE* in, sem_world* world) {
	if (in == NULL) return sem_set_error("File does not exist");

	if (read_dimensions(in, world) != SEM_OK) return SEM_ERROR;
	if (read_now(in, world) != SEM_OK) return SEM_ERROR;
	if (read_multiplier(in, world) != SEM_OK) return SEM_ERROR;
	if (read_tiles(in, world) != SEM_OK) return SEM_ERROR;
	if (read_trains(in, world) != SEM_OK) return SEM_ERROR;
	if (read_actions(in, world) != SEM_OK) return SEM_ERROR;

	return SEM_OK;
}

sem_success sem_serialize_save(FILE* out, sem_world* world) {
	fprintf(out, "world %d %d\n", world->max_x, world->max_y); // TODO: check status of fprintf
	if (write_timer(out, world) != SEM_OK) return SEM_ERROR;
	if (write_tiles(out, world) != SEM_OK) return SEM_ERROR;
	if (write_trains(out, world->trains) != SEM_OK) return SEM_ERROR;
	if (write_actions(out, world->actions) != SEM_OK) return SEM_ERROR;

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
		if (read_tile(in, world) != SEM_OK) return SEM_ERROR;
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
	if (sem_tile_parse(tile, &tokens, world->track_cache) != SEM_OK) return SEM_ERROR;

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
	free(line);

	for (uint32_t i=0; i < trains; i++) {
		if (read_train(in, world) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success read_train(FILE* in, sem_world* world) {
	sem_train* train = malloc(sizeof(sem_train));
	if (train == NULL) return sem_set_error("Failed to allocated memory for train");
	sem_train_init(train);
	train->world = world;

	if (read_train_id(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_name(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_state(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_direction(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_cars(in, train) != SEM_OK) return SEM_ERROR;

	sem_dynamic_array_add(world->trains, train);

	return SEM_OK;
}

sem_success read_train_id(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train identifier");
	// TODO: check first token is "train"

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	char* id_str = sem_tokenization_next(&tokens);
	uuid_parse(id_str, train->id);
	free(line);

	return SEM_OK;
}

sem_success read_train_name(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train name");
	// TODO: check first token is "name"

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	train->name = strdup(sem_tokenization_next(&tokens));
	free(line);

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
	if (strcmp(state, "stopped") == 0) {
		train->state = STOPPED;
	} else if (strcmp(state, "moving") == 0) {
		train->state = MOVING;
	} else if (strcmp(state, "derailed") == 0) {
		train->state = DERAILED;
	} else if (strcmp(state, "crashed") == 0) {
		train->state = CRASHED;
	} else {
		free(line);
		return sem_set_error("Unknown train state");
	}

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
	if (sem_tile_track_matching(tile, &key, &(car->track)) != SEM_OK) return SEM_ERROR;

	free(line);

	return sem_train_add_car(train, car);
}

sem_success read_actions(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read actions count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "actions"
	
	uint32_t actions = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	free(line);

	for (uint32_t i=0; i < actions; i++) {
		if (read_action(in, world) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success read_action(FILE* in, sem_world* world) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read action");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	uint32_t time = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	char* action_name = sem_tokenization_next(&tokens);

	sem_action_reader read = sem_action_reader_lookup(action_name);

	// FIXME: temporary guard because we don't serialize all types of action
	if (read != NULL) {
		sem_action* action;
		if (read(&tokens, world, &action) != SEM_OK) return SEM_ERROR;
		action->time = time;

		sem_heap_insert(world->actions, action);
	}

	return SEM_OK;
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

sem_success write_trains(FILE* out, sem_dynamic_array* trains) {
	fprintf(out, "trains %d\n", trains->tail_idx);

	for (uint32_t i=0; i<trains->tail_idx; i++) {
		if (write_train(out, trains->items[i]) != SEM_OK) return SEM_ERROR;
	}	

	return SEM_OK;
}

sem_success write_train(FILE* out, sem_train* train) {
	char id_str[37];
	uuid_unparse(train->id, id_str);
	fprintf(out, "train %s\n", id_str);
	fprintf(out, "name %s\n", train->name);
	if (write_train_state(out, train->state) != SEM_OK) return SEM_ERROR;
	fprintf(out, "direction ");
	if (sem_print_endpoint(out, train->direction) != SEM_OK) return SEM_ERROR;
	fprintf(out, "\n");

	fprintf(out, "cars %d\n", train->cars);

	sem_car* car = train->head_car;
	while (car != NULL) {
		if (write_car(out, car) != SEM_OK) return SEM_ERROR;
		car = car->next;
	}		

	return SEM_OK;
}

sem_success write_train_state(FILE* out, sem_train_state state) {
	fprintf(out, "state ");
	switch (state) {
	case STOPPED:
		fprintf(out, "stopped");
		break;
	case MOVING:
		fprintf(out, "moving");
		break;
	case DERAILED:
		fprintf(out, "derailed");
		break;
	case CRASHED:
		fprintf(out, "crashed");
		break;
	}
	fprintf(out, "\n");
	return SEM_OK;
}

sem_success write_car(FILE* out, sem_car* car) {	
	fprintf(out, "%d %d ", car->position.x, car->position.y);
	sem_print_track_part(out, car->track);
	fprintf(out, "\n");
	return SEM_OK;
}

sem_success write_actions(FILE* out, sem_dynamic_array* actions) {
	fprintf(out, "actions %u\n", sem_heap_size(actions));
	for (uint32_t i=0; i<sem_heap_size(actions); i++) {
		if (write_action(out, sem_heap_peek(actions, i)) != SEM_OK) return SEM_ERROR;
	}
	return SEM_OK;
}

sem_success write_action(FILE* out, sem_action* action) {
	fprintf(out, "%lu ", action->time);
	// FIXME: temporary until we're serializing all types of action
	if (action->write == NULL) {
		fprintf(out, "unknown_action");
	} else {
		if (action->write(out, action) != SEM_OK) return SEM_ERROR;
	}
	fprintf(out, "\n");
	return SEM_OK;
}
