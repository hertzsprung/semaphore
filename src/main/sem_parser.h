#ifndef _SEM_PARSER_H
#define _SEM_PARSER_H

#include "sem_error.h"
#include "sem_world.h"

sem_success sem_tile_parse(sem_tile* tile, char* tile_description, sem_track_cache* track_cache);

sem_success sem_track_parse(sem_track* track, char* track_description);

#endif

