#include <assert.h>
#include <stdlib.h>

#include "sem_input.h"
#include "sem_portal.h"
#include "sem_serialize_actions.h"
#include "sem_train.h"
#include "sem_world.h"

sem_success sem_train_entry_action(sem_dynamic_array* heap, sem_action* action) {
	sem_train_entry_context* context = (sem_train_entry_context*) action->context;
	
	sem_train* train = malloc(sizeof(sem_train));
	if (train == NULL) return sem_set_error("Failed to allocate memory for train");

	sem_train_init(train);
	train->state = MOVING;
	train->portal_state = ENTERING;
	train->direction = context->direction;
	train->entry_position = context->position;
	train->spawn_cars_remaining = context->cars;
	if (sem_world_add_train(&(action->game->world), train) != SEM_OK) return SEM_ERROR;
	if (sem_portal_spawn_car(train) != SEM_OK) return SEM_ERROR;

	action->time += 1000L;
	action->function = sem_move_train_action;
	action->write = sem_move_train_action_write;
	action->context = train;
	sem_heap_insert(heap, action);

	return SEM_OK;
}

sem_success sem_train_exit_action(sem_dynamic_array* heap, sem_action* action) {
	#pragma unused(heap)
	sem_train* train = (sem_train*) action->context;
	if (train->has_exit_position && sem_coordinate_equal(*(train->position), train->exit_position)) {
		action->game->revenue.balance += 200;
	} else {
		action->game->revenue.balance -= 300;
	}
	return sem_world_remove_train(train);	
}

sem_success sem_portal_spawn_car(sem_train* train) {
	assert(train->spawn_cars_remaining > 0);

	sem_car* car = malloc(sizeof(sem_car));
	if (car == NULL) return sem_set_error("Failed to allocate memory for car");

	car->position = train->entry_position;
	car->track = sem_tile_at_coord(train->world, &(car->position))->track;
	sem_train_add_car(train, car);
	train->spawn_cars_remaining--;
	return SEM_OK;
}
