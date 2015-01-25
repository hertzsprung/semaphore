#include <stdbool.h>
#include <stdlib.h>

#include "sem_input.h"

#include "sem_error.h"
#include "sem_heap.h"
#include "sem_world.h"

sem_success change_train_state(sem_heap* heap, sem_action* action);
sem_success move_train_action(sem_heap* heap, sem_action* action);

sem_success sem_train_input_act_upon(sem_input_event* input, sem_world* world, sem_action** action) {
	if (input->x == world->train->x && input->y == world->train->y) {
		// TODO: need to free() this somewhere
		*action = malloc(sizeof(sem_action)); // TODO: check return
		(*action)->time = input->time;
		(*action)->context = world->train;
		(*action)->function = change_train_state;
	}

	return SEM_OK;
}

sem_success change_train_state(sem_heap* heap, sem_action* change_state) {
	((sem_train*) change_state->context)->moving ^= true;

	// TODO: need to free() somewhere
	sem_action* move = malloc(sizeof(sem_action)); // TODO: check return
	move->time = change_state->time;
	move->context = change_state->context;
	move->function = move_train_action;

	sem_heap_insert(heap, move);

	return SEM_OK;
}

sem_success move_train_action(sem_heap* heap, sem_action* action) {
	action->time += 1000L;

	sem_train* train = (sem_train*) action->context;
	if (train->moving) {
		train->x++;
		sem_heap_insert(heap, action);
	}

	return SEM_OK;
}
