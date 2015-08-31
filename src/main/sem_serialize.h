#ifndef SEM_SERIALIZE_H
#define SEM_SERIALIZE_H

#include <stdio.h>
#include "sem_error.h"
#include "sem_game.h"

sem_success sem_serialize_load(FILE* in, sem_game* game);
sem_success sem_serialize_save(FILE* out, sem_game* game);

#endif
