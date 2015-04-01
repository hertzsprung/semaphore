#include "sem_parser.h"
#include "sem_strings.h"
#include <stdio.h>
#include <string.h>

sem_success track_set_endpoint(unit_vector* endpoint, char* description);

sem_success sem_tile_parse(sem_tile* tile, char* tile_description, sem_track_cache* track_cache) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, tile_description, " ");
	sem_tokenization_next(&tokens); // TODO: check token is "track"

	tile->class = TRACK;
	return sem_track_cache_find(track_cache, sem_tokenization_next(&tokens), &(tile->track));
}

sem_success sem_track_parse(sem_track* track, char* track_description) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, "-");

	track_set_endpoint(&(track->start), sem_tokenization_next(&tokens));
	track_set_endpoint(&(track->end), sem_tokenization_next(&tokens));

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
