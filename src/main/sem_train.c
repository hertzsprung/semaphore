#include <stdlib.h>

#include "sem_train.h"

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

sem_success sem_train_init(sem_train* train) {
	train->moving = false;	
	train->direction = 0;
	train->cars = malloc(sizeof(sem_dynamic_array));
	if (train->cars == NULL) {
		return sem_set_error("Could not allocate memory for train cars");
	}
	return sem_dynamic_array_init(train->cars);
}

sem_success sem_train_move(sem_train* train) {
	train->position->x += SEM_COMPASS_X(train->direction);
	train->position->y += SEM_COMPASS_Y(train->direction);

	sem_tile* tile = sem_tile_at_coord(train->world, train->position);
	return sem_tile_redirect(train, tile); // TODO: check return
}

sem_success sem_train_add_car(sem_train* train, sem_coordinate* car) {
	train->position = car; // TODO: only for the first car
	sem_dynamic_array_add(train->cars, car);
	return SEM_OK;
}

void sem_train_destroy(sem_train* train) {
	sem_dynamic_array_destroy(train->cars);
	free(train->cars);
}	
