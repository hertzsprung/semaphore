#ifndef SEM_SERIALIZE_RAILPRO_H
#define SEM_SERIALIZE_RAILPRO_H

#include <stdio.h>

#include "sem_error.h"
#include "sem_world.h"

sem_success sem_serialize_load_railpro(FILE* in, sem_world* world);

#endif

