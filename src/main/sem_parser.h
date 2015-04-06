#ifndef _SEM_PARSER_H
#define _SEM_PARSER_H

#include "sem_error.h"
#include "sem_strings.h"
#include "sem_world.h"

sem_success sem_tile_parse(sem_tile* tile, sem_tokenization* tile_description, sem_track_cache* track_cache);

sem_success sem_track_parse(char* track_description, sem_track** track);

sem_success sem_parse_track_part(sem_track* track, char* track_part_description);

sem_success sem_parse_unit_vector(unit_vector* vector, char* description);

#endif

