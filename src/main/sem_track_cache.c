#include <stdlib.h>

#include "sem_track_cache.h"

sem_success sem_track_cache_init(sem_track_cache* track_cache) {
	#pragma unused(track_cache)
	return SEM_OK;
}

sem_success sem_track_cache_find(sem_track_cache* track_cache, char* track_description, sem_track** track) {
	#pragma unused(track_cache)
	#pragma unused(track_description)

	*track = malloc(sizeof(sem_track));
	if (*track == NULL) return sem_set_error("Failed to allocated memory for track");
	return SEM_OK;
}

void sem_track_cache_destroy(sem_track_cache* track_cache) {
	#pragma unused(track_cache)
}
