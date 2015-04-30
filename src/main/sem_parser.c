#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sem_parser.h"
#include "sem_strings.h"
#include "sem_world.h"

sem_success sem_parse_signal(sem_tile* tile, sem_tokenization* tile_description);
sem_success sem_print_endpoint(FILE* out, unit_vector endpoint);

sem_success sem_tile_parse(sem_tile* tile, sem_tokenization* tile_description, sem_track_cache* track_cache) {
	char* class = sem_tokenization_next(tile_description);

	if (strcmp(class, "track") == 0) {
		tile->class = TRACK;
	} else if (strcmp(class, "points") == 0) {
		tile->class = POINTS;
	} else if (strcmp(class, "signal") == 0) {
		tile->class = SIGNAL;
	} else {
		return sem_set_error("Unknown tile class");
	}

	if (sem_track_cache_find(track_cache, sem_tokenization_next(tile_description), &(tile->track)) != SEM_OK) return SEM_ERROR;

	if (tile->class == POINTS) {
		sem_tile_set_points(tile, tile->track);
		sem_track_cache_find(track_cache, sem_tokenization_next(tile_description), &(tile->points[1]));
		// TODO: three-way points
	} else if (tile->class == SIGNAL) {
		if (sem_parse_signal(tile, tile_description) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success sem_track_parse(char* track_description, sem_track** track) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, "+");

	sem_track* previous_track = NULL;
	sem_track* current_track;
	char* track_part_description = sem_tokenization_next(&tokens);

	while (track_part_description != NULL) {
		current_track = malloc(sizeof(sem_track));
		if (current_track == NULL) return sem_set_error("Failed to allocated memory for track");

		sem_parse_track_part(current_track, track_part_description);

		if (previous_track == NULL) {
			*track = current_track;
		} else {
			previous_track->next = current_track;
		}

		previous_track = current_track;
		track_part_description = sem_tokenization_next(&tokens);
	}

	return SEM_OK;
}

sem_success sem_parse_track_part(sem_track* track, char* track_part_description) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_part_description, "-");

	unit_vector start, end;
	sem_parse_unit_vector(&start, sem_tokenization_next(&tokens));
	sem_parse_unit_vector(&end, sem_tokenization_next(&tokens));

	sem_track_set(track, start, end);

	return SEM_OK;
}

sem_success sem_parse_unit_vector(unit_vector* vector, char* description) {
	if (strchr(description, 'S') != NULL) {
		(*vector) = SEM_SOUTH;
	} else if (strchr(description, 'N') != NULL) {
		(*vector) = SEM_NORTH;
	} else {
		(*vector) = 0;
	}

	if (strchr(description, 'E') != NULL) {
		(*vector) |= SEM_EAST;
	} else if (strchr(description, 'W') != NULL) {
		(*vector) |= SEM_WEST;
	}

	return SEM_OK;
}

sem_success sem_parse_signal(sem_tile* tile, sem_tokenization* tile_description) {
	sem_signal* signal = malloc(sizeof(sem_signal));
	if (signal == NULL) return sem_set_error("Failed to allocated memory for signal");
	tile->signal = signal;

	char* aspect = sem_tokenization_next(tile_description);
	if (strcmp(aspect, "green") == 0) {
		signal->aspect = GREEN;
	} else if (strcmp(aspect, "amber") == 0) {
		signal->aspect = AMBER;
	} else if (strcmp(aspect, "red") == 0) {
		signal->aspect = RED;
	} else {
		return sem_set_error("Unknown signal aspect");
	}

	char* primary_type = sem_tokenization_next(tile_description);
	if (strcmp(primary_type, "main") == 0) {
		char* secondary_type = sem_tokenization_next(tile_description);
		if (strcmp(secondary_type, "manual") == 0) {
			signal->type = MAIN_MANUAL;
		} else if (strcmp(secondary_type, "auto") == 0) {
			signal->type = MAIN_AUTO;
		} else {
			return sem_set_error("Unknown main signal subtype");
		}
	} else if (strcmp(primary_type, "sub") == 0) {
		signal->type = SUB;
	} else {
		return sem_set_error("Unknown signal type");
	}
	
	return SEM_OK;
}

sem_success sem_tile_print(FILE* out, sem_tile* tile) {
	fprintf(out, "track "); // TODO: check returns

	sem_track* t = tile->track;
	do {
		if (sem_print_endpoint(out, t->start) != SEM_OK) return SEM_ERROR;
		fprintf(out, "-");
		if (sem_print_endpoint(out, t->end) != SEM_OK) return SEM_ERROR;

		t = t->next;
		if (t != NULL) fprintf(out, "+");
	} while (t != NULL);

	return SEM_OK;
}

sem_success sem_print_endpoint(FILE* out, unit_vector endpoint) {
	if (SEM_COMPASS_Y(endpoint) < 0) {
		fprintf(out, "N"); // TODO: check returns
	} else if (SEM_COMPASS_Y(endpoint) > 0) {
		fprintf(out, "S");
	}

	if (SEM_COMPASS_X(endpoint) > 0) {
		fprintf(out, "E");
	} else if (SEM_COMPASS_X(endpoint) < 0) {
		fprintf(out, "W");
	}
		
	return SEM_OK;
}
