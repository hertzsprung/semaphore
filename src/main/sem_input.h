#ifndef _SEM_INPUT_H
#define _SEM_INPUT_H

#include <stdint.h>

#include "sem_error.h"
#include "sem_heap.h"
#include "sem_world.h"

typedef struct {
	uint32_t x, y; /**< tile coordinate */
} sem_input_event;

typedef sem_success (*sem_input_handler)(sem_input_event* input, sem_world* world, sem_action** action);

// TODO: split this function into sem_train_input.h otherwise creating a circular dependency between sem_train.h and sem_world.h
sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action);

#endif
