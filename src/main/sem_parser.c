#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sem_parser.h"
#include "sem_strings.h"
#include "sem_world.h"

sem_success track_set_endpoint(unit_vector* endpoint, char* description);
sem_success set_track_part(sem_track* track, char* track_part_description);

sem_success sem_tile_parse(sem_tile* tile, sem_tokenization* tile_description, sem_track_cache* track_cache) {
	sem_tokenization_next(tile_description); // TODO: check token is "track"

	tile->class = TRACK;
	return sem_track_cache_find(track_cache, sem_tokenization_next(tile_description), &(tile->track));
}

sem_success sem_track_parse(char* track_description, sem_track** track) {
	sem_tokenization track_tokens;
	sem_tokenization_init(&track_tokens, track_description, "+");

	sem_track* previous_track = NULL;
	sem_track* current_track;
	char* track_part_description = sem_tokenization_next(&track_tokens);

	while (track_part_description != NULL) {
		current_track = malloc(sizeof(sem_track));
		if (current_track == NULL) return sem_set_error("Failed to allocated memory for track");

		set_track_part(current_track, track_part_description);

		if (previous_track == NULL) {
			*track = current_track;
		} else {
			previous_track->next = current_track;
		}

		previous_track = current_track;
		track_part_description = sem_tokenization_next(&track_tokens);
	}

	return SEM_OK;
}

sem_success set_track_part(sem_track* track, char* track_part_description) {
	sem_tokenization endpoint_tokens;
	sem_tokenization_init(&endpoint_tokens, track_part_description, "-");

	unit_vector start, end;
	char* start_str = sem_tokenization_next(&endpoint_tokens);
	char* end_str = sem_tokenization_next(&endpoint_tokens);
	track_set_endpoint(&start, start_str);
	track_set_endpoint(&end, end_str);

	sem_track_set(track, start, end);

	return SEM_OK;
}

sem_success track_set_endpoint(unit_vector* endpoint, char* description) {
	if (strchr(description, 'S') != NULL) {
		(*endpoint) = SEM_SOUTH;
	} else if (strchr(description, 'N') != NULL) {
		(*endpoint) = SEM_NORTH;
	} else {
		(*endpoint) = 0;
	}

	if (strchr(description, 'E') != NULL) {
		(*endpoint) |= SEM_EAST;
	} else if (strchr(description, 'W') != NULL) {
		(*endpoint) |= SEM_WEST;
	}

	return SEM_OK;
}
