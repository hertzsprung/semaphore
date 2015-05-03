#ifndef SEM_TRACK_CACHE_H
#define SEM_TRACK_CACHE_H

#include <glib.h>

typedef struct sem_track_cache sem_track_cache;

#include "sem_error.h"
#include "sem_world.h"

struct sem_track_cache {
	GHashTable* table;
}; 

sem_success sem_track_cache_init(sem_track_cache* track_cache);

sem_success sem_track_cache_find(sem_track_cache* track_cache, char* track_description, sem_track** track);

void sem_track_cache_destroy(sem_track_cache* track_cache);

#endif

