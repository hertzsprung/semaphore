#ifndef _SEM_SERIALIZE_H
#define _SEM_SERIALIZE_H

#include <stdio.h>
#include "sem_error.h"
#include "sem_world.h"

sem_success sem_serialize_load(FILE* in, sem_world* world);

#endif
