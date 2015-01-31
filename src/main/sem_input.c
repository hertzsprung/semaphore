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
	if (input->x == world->train->x && input->y == world->train->y) {
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

sem_success change_train_state(sem_heap* heap, sem_action* change_state) {
	sem_train* train = (sem_train*) change_state->context;

	train->moving ^= true;
	if (train->moving) {
		sem_action* move = sem_action_new();
		if (move == NULL) return SEM_ERROR;
		move->time = change_state->time;
		move->context = change_state->context;
		move->function = move_train_action;

		sem_heap_insert(heap, move);
	}

	return SEM_OK;
}

sem_success move_train_action(sem_heap* heap, sem_action* action) {
	sem_train* train = (sem_train*) action->context;
	if (train->moving) {
		sem_train_move(train); // TODO: check return

		sem_action* next_move = sem_action_new();
		if (next_move == NULL) return SEM_ERROR;
		next_move->time = action->time + 1000L;
		next_move->context = train;
		next_move->function = move_train_action;
		sem_heap_insert(heap, next_move);
	}

	return SEM_OK;
}
