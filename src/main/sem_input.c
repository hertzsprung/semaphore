#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_input.h"

#include "sem_action.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_portal.h"
#include "sem_serialize_actions.h"
#include "sem_train.h"
#include "sem_world.h"

sem_train* train_occupying_tile(sem_dynamic_array* trains, sem_coordinate* tile);
sem_success switch_points_action(sem_dynamic_array* heap, sem_action* action);
sem_success change_train_state(sem_dynamic_array* heap, sem_action* action);
sem_success reverse_train(sem_dynamic_array* heap, sem_action* action);
sem_success toggle_signal_aspect(sem_dynamic_array* heap, sem_action* action);
sem_success set_signal_to_amber(sem_dynamic_array* heap, sem_action* action);
sem_success release_any_held_train(sem_signal* signal, sem_dynamic_array* heap, sem_action* action);

sem_success sem_tile_input_act_upon(sem_input_event* input, sem_game* game, sem_action** action) {
	sem_tile* tile = sem_tile_at_coord(&(game->world), input->tile);

	if (tile->class == POINTS) {
		*action = sem_action_new(game);
		if (*action == NULL) return SEM_ERROR;
		(*action)->time = input->time;
		(*action)->context = tile;
		(*action)->function = switch_points_action;
	} else if (tile->class == SIGNAL) {
		*action = sem_action_new(game);
		if (*action == NULL) return SEM_ERROR;
		(*action)->time = input->time;
		(*action)->context = tile->signal;

		switch (input->rank) {
		case PRIMARY:
			(*action)->function = toggle_signal_aspect;
			break;
		case SECONDARY:
			(*action)->function = set_signal_to_amber;
			break;
		}
	}
	return SEM_OK;
}

sem_success switch_points_action(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_tile* tile = (sem_tile*) action->context;
	sem_tile_switch_points(tile);
	return SEM_OK;
}

sem_success sem_train_input_act_upon(sem_input_event* input, sem_game* game, sem_action** action) {
	sem_train* train = train_occupying_tile(game->world.trains, input->tile);
	if (train == NULL) return SEM_OK;

	*action = sem_action_new(game);
	if (*action == NULL) return SEM_ERROR;
	(*action)->time = input->time;
	(*action)->context = train;

	switch (input->rank) {
	case PRIMARY:
		(*action)->function = change_train_state;
		break;
	case SECONDARY:
		(*action)->function = reverse_train;
		break;
	}

	return SEM_OK;
}

sem_train* train_occupying_tile(sem_dynamic_array* trains, sem_coordinate* tile) {
	for (uint32_t i=0; i < trains->tail_idx; i++) {
		if (sem_train_occupies(trains->items[i], tile))  return trains->items[i];
	}

	return NULL;
}

sem_success change_train_state(sem_dynamic_array* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;

	if (train->state == STOPPED) {
		train->state = MOVING;
	} else if (train->state == MOVING) {
		train->state = STOPPED;
	}

	if (train->state == MOVING) {
		action->function = sem_move_train_action;
		action->write = sem_move_train_action_write;

		if (sem_heap_insert(heap, action) != SEM_OK) return SEM_ERROR;
	}

	return SEM_OK;
}

sem_success sem_move_train_action(sem_dynamic_array* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;
	if (train->state == MOVING) {
		if (train->portal_state == EXITING) {
			if (train->cars == 1) {
				action->function = sem_train_exit_action;
				return sem_heap_insert(heap, action);
			} else {
				sem_train_remove_head_car(train);
			}
		}

		sem_train_move_outcome outcome;
		if (sem_train_move(train, &outcome) != SEM_OK) return SEM_ERROR;

		if (train->state == DERAILED) {
			action->time += 10000L;
			action->function = remove_train_action;
			action->write = sem_remove_train_action_write;
			if (sem_heap_insert(heap, action) != SEM_OK) return SEM_ERROR;
		} else if (train->state == MOVING) {
			if (train->portal_state == ENTERING) {
				if (sem_portal_spawn_car(train) != SEM_OK) return SEM_ERROR;
				if (train->spawn_cars_remaining == 0) train->portal_state = ENTERED;
			}
			action->time += train->speeds[train->speed];
			action->function = sem_move_train_action;
			action->write = sem_move_train_action_write;
			if (sem_heap_insert(heap, action) != SEM_OK) return SEM_ERROR;
		}

		if (outcome.stopped_at_buffer) {
			action->time += 5000L;
			action->function = sem_reverse_train_at_buffer_action;
			action->write = sem_reverse_train_at_buffer_action_write;
			if (sem_heap_insert(heap, action) != SEM_OK) return SEM_ERROR;
		}
	}

	return SEM_OK;
}

sem_success reverse_train(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_train* train = (sem_train*) action->context;
	sem_train_reverse(train);
	
	return SEM_OK;
}

sem_success sem_reverse_train_at_buffer_action(sem_dynamic_array* heap, sem_action* action) {
	if (reverse_train(heap, action) != SEM_OK) return SEM_ERROR;
	((sem_train*) action->context)->state = MOVING;
	if (sem_move_train_action(heap, action) != SEM_OK) return SEM_ERROR;

	return SEM_OK;
}

sem_success remove_train_action(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_train* train = (sem_train*) action->context;
	sem_dynamic_array_remove(train->world->trains, train);
	return SEM_OK;
}

sem_success toggle_signal_aspect(sem_dynamic_array* heap, sem_action* action) {
	sem_signal* signal = (sem_signal*) action->context;
	signal->aspect = (signal->aspect == GREEN) ? RED : GREEN;
	return release_any_held_train(signal, heap, action);
}

sem_success set_signal_to_amber(sem_dynamic_array* heap, sem_action* action) {
	sem_signal* signal = (sem_signal*) action->context;
	signal->aspect = AMBER;
	return release_any_held_train(signal, heap, action);
}

sem_success release_any_held_train(sem_signal* signal, sem_dynamic_array* heap, sem_action* action) {
	if (sem_signal_holding_train(signal)) {
		action->function = change_train_state;
		action->context = sem_signal_release_train(signal);
		return sem_heap_insert(heap, action);
	} else {
		return SEM_OK;
	}
}
