#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_serialize.h"
#include "sem_serialize_actions.h"
#include "sem_error.h"
#include "sem_game.h"
#include "sem_heap.h"
#include "sem_parser.h"
#include "sem_strings.h"

sem_success read_dimensions(FILE* in, sem_game* game);
sem_success read_now(FILE* in, sem_world* world);
sem_success read_multiplier(FILE* in, sem_world* world);
sem_success read_tiles(FILE* in, sem_world* world);
sem_success read_tile(FILE* in, sem_world* world);
sem_success read_trains(FILE* in, sem_world* world);
sem_success read_train(FILE* in, sem_world* world);
sem_success read_train_id(FILE* in, sem_train* train);
sem_success read_train_name(FILE* in, sem_train* train);
sem_success read_train_state(FILE* in, sem_train* train);
sem_success read_train_portal_state(FILE* in, sem_train* train);
sem_success read_train_spawn_cars_remaining(FILE* in, sem_train* train);
sem_success read_train_entry_position(FILE* in, sem_train* train);
sem_success read_train_exit_position(FILE* in, sem_train* train);
sem_success read_train_headless(FILE* in, sem_train* train);
sem_success read_train_direction(FILE* in, sem_train* train);
sem_success read_train_cars(FILE* in, sem_train* train);
sem_success read_car(FILE* in, sem_train* train);
sem_success read_labels(FILE* in, sem_game* game);
sem_success read_label(FILE* in, sem_game* game);
sem_success read_actions(FILE* in, sem_game* game);
sem_success read_action(FILE* in, sem_game* game);
sem_success read_revenue(FILE* in, sem_revenue* revenue);

sem_success write_timer(FILE* out, sem_world* world);
sem_success write_tiles(FILE* out, sem_world* world);
sem_success write_tile(FILE* out, uint32_t x, uint32_t y, sem_tile* tile);
sem_success write_trains(FILE* out, sem_dynamic_array* trains);
sem_success write_train(FILE* out, sem_train* train);
sem_success write_train_state(FILE* out, sem_train_state state);
sem_success write_train_portal_state(FILE* out, sem_train_portal_state state);
sem_success write_train_exit_position(FILE* out, sem_train* train);
sem_success write_car(FILE* out, sem_car* car);
sem_success write_labels(FILE* out, sem_dynamic_array* labels);
sem_success write_label(FILE* out, sem_label* label);
sem_success write_actions(FILE* out, sem_dynamic_array* actions);
sem_success write_action(FILE* out, sem_action* action);
sem_success write_revenue(FILE* out, sem_revenue* revenue);

sem_success sem_serialize_load(FILE* in, sem_game* game) {
	if (in == NULL) return sem_set_error("File does not exist");

	sem_world* world = &(game->world);
	if (read_dimensions(in, game) != SEM_OK) return SEM_ERROR;
	if (read_now(in, world) != SEM_OK) return SEM_ERROR;
	if (read_multiplier(in, world) != SEM_OK) return SEM_ERROR;
	if (read_tiles(in, world) != SEM_OK) return SEM_ERROR;
	if (read_trains(in, world) != SEM_OK) return SEM_ERROR;
	if (read_labels(in, game) != SEM_OK) return SEM_ERROR;
	if (read_actions(in, game) != SEM_OK) return SEM_ERROR;
	if (read_revenue(in, &(game->revenue)) != SEM_OK) return SEM_ERROR;

	return SEM_OK;
}

sem_success sem_serialize_save(FILE* out, sem_game* game) {
	sem_world* world = &(game->world);

	fprintf(out, "world %d %d\n", world->max_x, world->max_y); // TODO: check status of fprintf
	if (write_timer(out, world) != SEM_OK) return SEM_ERROR;
	if (write_tiles(out, world) != SEM_OK) return SEM_ERROR;
	if (write_trains(out, world->trains) != SEM_OK) return SEM_ERROR;
	if (write_labels(out, game->labels) != SEM_OK) return SEM_ERROR;
	if (write_actions(out, world->actions) != SEM_OK) return SEM_ERROR;
	if (write_revenue(out, &(game->revenue)) != SEM_OK) return SEM_ERROR;

	return SEM_OK;
}

sem_success read_dimensions(FILE* in, sem_game* game) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read world dimensions");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "world"
	
	game->world.max_x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	game->world.max_y = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	free(line);

	return sem_game_init_blank(game);
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
	if (train == NULL) return sem_set_error("Failed to allocate memory for train");
	sem_train_init(train);
	train->world = world;

	if (read_train_id(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_name(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_state(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_portal_state(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_spawn_cars_remaining(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_entry_position(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_exit_position(in, train) != SEM_OK) return SEM_ERROR;
	if (read_train_headless(in, train) != SEM_OK) return SEM_ERROR;
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

sem_success read_train_portal_state(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read train portal_state");
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "portal_state"

	char* state = sem_tokenization_next(&tokens);
	if (strcmp(state, "entering") == 0) {
		train->portal_state = ENTERING;
	} else if (strcmp(state, "entered") == 0) {
		train->portal_state = ENTERED;
	} else if (strcmp(state, "exiting") == 0) {
		train->portal_state = EXITING;
	} else {
		free(line);
		return sem_set_error("Unknown train portal state");
	}

	free(line);

	return SEM_OK;
}

sem_success read_train_spawn_cars_remaining(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read spawn_cars_remaining");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "spawn_cars_remaining"

	train->spawn_cars_remaining = sem_parse_uint8_t(sem_tokenization_next(&tokens));

	free(line);

	return SEM_OK;
}

sem_success read_train_entry_position(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read entry_position");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "entry_position"

	train->entry_position.x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	train->entry_position.y = sem_parse_uint32_t(sem_tokenization_next(&tokens));

	free(line);

	return SEM_OK;
}

sem_success read_train_exit_position(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read exit_position");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "exit_position"

	if (strcmp(sem_tokenization_next(&tokens), "none") == 0) {
		train->has_exit_position = false;
	} else {
		train->has_exit_position = true;
		train->exit_position.x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
		train->exit_position.y = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	}

	free(line);

	return SEM_OK;
}

sem_success read_train_headless(FILE* in, sem_train* train) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read headless flag");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "headless"

	train->headless = strcmp(sem_tokenization_next(&tokens), "true") == 0;

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
	if (car == NULL) return sem_set_error("Failed to allocate memory for car");

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

	sem_train_add_car(train, car);
	return SEM_OK;
}

sem_success read_labels(FILE* in, sem_game* game) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read labels count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "labels"
	
	uint32_t labels = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	free(line);

	for (uint32_t i=0; i < labels; i++) {
		if (read_label(in, game) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success read_label(FILE* in, sem_game* game) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read label");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");

	sem_label* label = malloc(sizeof(sem_label));
	if (label == NULL) return sem_set_error("Failed to allocate memory for label");

	label->position.x = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	label->position.y = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	label->text = strdup(sem_tokenization_next(&tokens));
	free(line);

	if (sem_dynamic_array_add(game->labels, label) != SEM_OK) return SEM_ERROR;

	return SEM_OK;
}

sem_success read_actions(FILE* in, sem_game* game) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read actions count");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "actions"
	
	uint32_t actions = sem_parse_uint32_t(sem_tokenization_next(&tokens));
	free(line);

	for (uint32_t i=0; i < actions; i++) {
		if (read_action(in, game) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success read_action(FILE* in, sem_game* game) {
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
		if (read(&tokens, game, &action) != SEM_OK) return SEM_ERROR;
		action->time = time;

		sem_heap_insert(game->world.actions, action);
	}

	return SEM_OK;
}

sem_success read_revenue(FILE* in, sem_revenue* revenue) {
	char* line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read revenue");
	// TODO: check line is "revenue"
	free(line);

	line = sem_read_line(in);
	if (line == NULL) return sem_set_error("Could not read balance");

	sem_tokenization tokens;
	sem_tokenization_init(&tokens, line, " ");
	sem_tokenization_next(&tokens);
	// TODO: check token is "balance"
	
	revenue->balance = sem_parse_int32_t(sem_tokenization_next(&tokens));

	free(line);

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
	if (write_train_portal_state(out, train->portal_state) != SEM_OK) return SEM_ERROR;
	fprintf(out, "spawn_cars_remaining %d\n", train->spawn_cars_remaining);
	fprintf(out, "entry_position %d %d\n", train->entry_position.x, train->entry_position.y);
	if (write_train_exit_position(out, train) != SEM_OK) return SEM_ERROR;
	fprintf(out, "headless %s\n", train->headless ? "true" : "false");
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

sem_success write_train_portal_state(FILE* out, sem_train_portal_state state) {
	fprintf(out, "portal_state ");
	switch (state) {
	case ENTERING:
		fprintf(out, "entering");
		break;
	case ENTERED:
		fprintf(out, "entered");
		break;
	case EXITING:
		fprintf(out, "exiting");
		break;
	}
	fprintf(out, "\n");
	return SEM_OK;
}

sem_success write_train_exit_position(FILE* out, sem_train* train) {
	fprintf(out, "exit_position ");
	if (train->has_exit_position) {
		fprintf(out, "at %d %d\n", train->exit_position.x, train->exit_position.y);
	} else {
		fprintf(out, "none\n");
	}
	return SEM_OK;
}

sem_success write_car(FILE* out, sem_car* car) {	
	fprintf(out, "%d %d ", car->position.x, car->position.y);
	sem_print_track_part(out, car->track);
	fprintf(out, "\n");
	return SEM_OK;
}

sem_success write_labels(FILE* out, sem_dynamic_array* labels) {
	fprintf(out, "labels %u\n", labels->tail_idx);
	for (uint32_t i=0; i < labels->tail_idx; i++) {
		if (write_label(out, labels->items[i]) != SEM_OK) return SEM_ERROR;
	}
	return SEM_OK;
}

sem_success write_label(FILE* out, sem_label* label) {
	fprintf(out, "%d %d %s\n", label->position.x, label->position.y, label->text);
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

sem_success write_revenue(FILE* out, sem_revenue* revenue) {
	fprintf(out, "revenue\n");
	fprintf(out, "balance %d\n", revenue->balance);
	return SEM_OK;
}
