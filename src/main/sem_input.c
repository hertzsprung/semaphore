#include <stdbool.h>
#include <stdlib.h>

#include "sem_input.h"

#include "sem_action.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

sem_success change_train_state(sem_heap* heap, sem_action* action);
sem_success move_train_action(sem_heap* heap, sem_action* action);

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	if (sem_train_occupies(world->train, input->tile)) {
		*action = malloc(sizeof(sem_action));
		if (*action == NULL) {
			return sem_set_error("Could not create action");
		}
		(*action)->time = input->time;
		(*action)->context = world->train;
		(*action)->function = change_train_state;
		(*action)->dynamically_allocated = true;
	}

	return SEM_OK;
}

sem_success change_train_state(sem_heap* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;

	train->moving ^= true;
	if (train->moving) {
		action->function = move_train_action;

		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}

sem_success move_train_action(sem_heap* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;
	if (train->moving) {
		if (sem_train_move(train) != SEM_OK) return SEM_ERROR;

		action->time = action->time + 1000L;
		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}
