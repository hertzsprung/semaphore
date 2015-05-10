#ifndef SEM_INPUT_H
#define SEM_INPUT_H

#include <stdint.h>

#include "sem_compass.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_world.h"

typedef struct {
	uint64_t time; /**< game time */
	sem_coordinate* tile;
} sem_input_event;

typedef sem_success (*sem_input_handler)(sem_input_event* input, sem_world* world, sem_action** action);

sem_success sem_tile_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action);

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action);

sem_success move_train_action(sem_dynamic_array* heap, sem_action* action); // TODO: should this really be public? only using it for tests

#endif
