#ifndef _SEM_TRAIN_H
#define _SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>

typedef struct sem_train sem_train;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

struct sem_train {
	sem_world* world;
	bool moving;
	sem_coordinate* position; /**< position of the train's head car */
	sem_dynamic_array* cars;
	unit_vector direction;
};

sem_success sem_train_init(sem_train* train);

sem_success sem_train_move(sem_train* train);

sem_success sem_train_add_car(sem_train* train, sem_coordinate* car);

void sem_train_destroy(sem_train* train);

#endif
