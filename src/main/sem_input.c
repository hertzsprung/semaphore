#include <stdbool.h>
#include <stdlib.h>

#include "sem_input.h"

#include "sem_error.h"
#include "sem_heap.h"
#include "sem_world.h"

sem_success change_train_state(sem_heap* heap, sem_action* action);

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	if (input->x == world->train->x && input->y == world->train->y) {
		// TODO: need to free() this somewhere
		*action = malloc(sizeof(sem_action)); // TODO: check return
		(*action)->context = world->train;
		(*action)->function = change_train_state;
	}

	return SEM_OK;
}

sem_success change_train_state(sem_heap* heap, sem_action* action) {
	#pragma unused(heap)
	((sem_train*) action->context)->moving ^= true;
	// TODO: if train isn't moving then need to add an action to the heap

	return SEM_OK;
}
