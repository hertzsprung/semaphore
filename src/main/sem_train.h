#ifndef _SEM_TRAIN_H
#define _SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>

typedef struct sem_train sem_train;

#include "sem_compass.h"
#include "sem_error.h"
#include "sem_world.h"

struct sem_train {
	sem_world* world;
	bool moving;
	sem_coordinate* position;
	unit_vector direction;
};

sem_success sem_train_move(sem_train* train);

#endif
