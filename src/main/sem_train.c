#include <stdlib.h>
#include <stdbool.h>

#include "sem_train.h"

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

void train_move_trailing(sem_dynamic_array* cars);

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
	train_move_trailing(train->cars);
	train->position->x += SEM_COMPASS_X(train->direction);
	train->position->y += SEM_COMPASS_Y(train->direction);

	sem_tile* tile = sem_tile_at_coord(train->world, train->position);
	return sem_tile_redirect(train, tile); // TODO: check return
}

sem_success sem_train_add_car(sem_train* train, sem_coordinate* car) {
	if (train->cars->tail_idx == 0) {
		train->position = car;
	}
	sem_dynamic_array_add(train->cars, car);
	return SEM_OK;
}

bool sem_train_occupies(sem_train* train, sem_coordinate* tile) {
	return sem_coordinate_equal(train->position, tile);
}

void sem_train_destroy(sem_train* train) {
	sem_dynamic_array_destroy(train->cars);
	free(train->cars);
}	

void train_move_trailing(sem_dynamic_array* cars) {
	for (uint32_t i=cars->tail_idx-1; i > 0; i--) {
		sem_coordinate* car_behind = (sem_coordinate*) cars->items[i];
		sem_coordinate* car_in_front = (sem_coordinate*) cars->items[i-1];
		car_behind->x = car_in_front->x;
		car_behind->y = car_in_front->y;
	}
}
