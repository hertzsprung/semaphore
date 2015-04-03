#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sem_track_cache.h"
#include "sem_parser.h"

void key_destroy(gpointer key);
void track_destroy(gpointer ptr);

sem_success sem_track_cache_init(sem_track_cache* track_cache) {
	track_cache->table = g_hash_table_new_full(g_str_hash, g_str_equal, key_destroy, track_destroy);
	return SEM_OK;
}

sem_success sem_track_cache_find(sem_track_cache* track_cache, char* track_description, sem_track** track) {
	*track = g_hash_table_lookup(track_cache->table, track_description);

	if (*track == NULL) {
		char* key = strdup(track_description);
		sem_track_parse(track_description, track);

		g_hash_table_insert(track_cache->table, key, *track);
	}

	return SEM_OK;
}

void sem_track_cache_destroy(sem_track_cache* track_cache) {
	g_hash_table_destroy(track_cache->table);
}

void key_destroy(gpointer key) {
	free(key);
}

void track_destroy(gpointer ptr) {
	sem_track* track = (sem_track*) ptr;
	if (track->next != NULL) track_destroy(track->next);
	free(track);
}
