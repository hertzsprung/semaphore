#include "sem_tile_parser.h"
#include "sem_strings.h"
#include <stdio.h>

sem_success sem_tile_parse(char* string, sem_track_cache* track_cache, sem_tile* tile) {
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, string);
	sem_tokenization_next(&tokens); // TODO: check token is "track"

	tile->class = TRACK;
	sem_track_cache_find(track_cache, sem_tokenization_next(&tokens), &(tile->track)); // TODO: check return

	return SEM_OK;
}
