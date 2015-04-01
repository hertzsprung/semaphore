#include <string.h>

#include "sem_parser.h"
#include "sem_strings.h"
#include "sem_world.h"

sem_success track_set_endpoint(unit_vector* endpoint, char* description);

sem_success sem_tile_parse(sem_tile* tile, sem_tokenization* tile_description, sem_track_cache* track_cache) {
	sem_tokenization_next(tile_description); // TODO: check token is "track"

	tile->class = TRACK;
	return sem_track_cache_find(track_cache, sem_tokenization_next(tile_description), &(tile->track));
}

sem_success sem_track_parse(sem_track* track, char* track_description) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, "-");

	unit_vector start, end;
	track_set_endpoint(&start, sem_tokenization_next(&tokens));
	track_set_endpoint(&end, sem_tokenization_next(&tokens));

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
