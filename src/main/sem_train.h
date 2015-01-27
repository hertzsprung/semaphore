#ifndef _SEM_TRAIN_H
#define _SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>

#include "sem_error.h"

typedef uint8_t unit_vector;

typedef struct {
	bool moving;
	uint32_t x, y;
    unit_vector velocity;
} sem_train;

sem_success sem_train_move(sem_train* train);

#endif
