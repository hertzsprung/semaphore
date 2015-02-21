#include <stdbool.h>
#include <stdlib.h>

#include "sem_input.h"

#include "sem_action.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

sem_success change_train_state(sem_dynamic_array* heap, sem_action* action);
sem_success move_train_action(sem_dynamic_array* heap, sem_action* action);

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	for (uint32_t i=0; i < world->trains->tail_idx; i++) {
		if (sem_train_occupies(world->trains->items[i], input->tile)) {
			*action = malloc(sizeof(sem_action));
			if (*action == NULL) {
				return sem_set_error("Could not create action");
			}
			(*action)->time = input->time;
			(*action)->context = world->trains->items[i];
			(*action)->function = change_train_state;
			(*action)->dynamically_allocated = true;
		}
	}

	return SEM_OK;
}

sem_success change_train_state(sem_dynamic_array* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;

	if (train->state == STOPPED) {
		train->state = MOVING;
	} else {
		train->state = STOPPED;
	}

	if (train->state == MOVING) {
		action->function = move_train_action;

		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}

sem_success move_train_action(sem_dynamic_array* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;
	if (train->state == MOVING) {
		if (sem_train_move(train) != SEM_OK) return SEM_ERROR;

		action->time = action->time + 1000L;
		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}
