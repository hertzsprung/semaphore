#include <stdlib.h>

#include "sem_input.h"
#include "sem_portal.h"
#include "sem_serialize_actions.h"
#include "sem_train.h"
#include "sem_world.h"

sem_success sem_train_entry_action(sem_dynamic_array* heap, sem_action* action) {
	sem_train_entry_context* context = (sem_train_entry_context*) action->context;
	sem_world* world = context->world;
	
	sem_train* train = malloc(sizeof(sem_train));
	if (train == NULL) return sem_set_error("Failed to allocate memory for train");

	sem_train_init(train);
	train->state = MOVING;
	train->direction = context->direction;

	sem_car* car = malloc(sizeof(sem_car));
	if (car == NULL) return sem_set_error("Failed to allocate memory for car");

	car->position = context->position;
	car->track = sem_tile_at_coord(world, &(car->position))->track;
	sem_train_add_car(train, car);
	
	sem_world_add_train(world, train);

	action->function = sem_move_train_action;
	action->write = sem_move_train_action_write;
	action->context = train;
	sem_heap_insert(heap, action);

	return SEM_OK;
}
