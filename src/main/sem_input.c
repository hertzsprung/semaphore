#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_input.h"

#include "sem_action.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

sem_success switch_points_action(sem_dynamic_array* heap, sem_action* action);
sem_success change_train_state(sem_dynamic_array* heap, sem_action* action);

sem_success sem_tile_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	sem_tile* tile = sem_tile_at_coord(world, input->tile);

	if (tile->class == POINTS) {
		*action = sem_action_new();
		if (*action == NULL) return SEM_ERROR;
		(*action)->time = input->time;
		(*action)->context = tile;
		(*action)->function = switch_points_action;
	}
	return SEM_OK;
}

sem_success switch_points_action(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_tile* tile = (sem_tile*) action->context;
	sem_tile_switch_points(tile);
	return SEM_OK;
}

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	for (uint32_t i=0; i < world->trains->tail_idx; i++) {
		// TODO: if it's legitimate for mulitple trains to occupy the same tile then this input will act upon all of them
		// TODO: not an issue until we introduce layers
		if (sem_train_occupies(world->trains->items[i], input->tile)) {
			*action = sem_action_new();
			if (*action == NULL) return SEM_ERROR;
			(*action)->time = input->time;
			(*action)->context = world->trains->items[i];
			(*action)->function = change_train_state;
		}
	}

	return SEM_OK;
}

sem_success change_train_state(sem_dynamic_array* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;

	if (train->state == STOPPED) {
		train->state = MOVING;
	} else if (train->state == MOVING) {
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

		if (train->state == DERAILED) {
			action->time = action->time + 10000L;
			action->function = remove_train_action;
		} else {
			action->time = action->time + 1000L;
		}

		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}

sem_success remove_train_action(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_train* train = (sem_train*) action->context;
	sem_dynamic_array_remove(train->world->trains, train);
	return SEM_OK;
}
