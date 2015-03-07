#include <stdlib.h>
#include <stdbool.h>

#include "sem_train.h"

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

void train_move_trailing(sem_dynamic_array* cars);
sem_train* train_detect_collision(sem_train* train);

sem_success sem_train_init(sem_train* train) {
	train->state = STOPPED;
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

	sem_train* collided_train = train_detect_collision(train);
	if (collided_train != NULL) {
		train->state = CRASHED;
		collided_train->state = CRASHED;
	}

	sem_tile* tile = sem_tile_at_coord(train->world, train->position);
	sem_tile_acceptance acceptance;
	sem_success success = sem_tile_accept(train, tile, &acceptance);
	if (success != SEM_OK) return success;
	((sem_car*) train->cars->items[0])->track = acceptance.track;	
	return SEM_OK;
}

sem_success sem_train_add_car(sem_train* train, sem_car* car) {
	if (train->cars->tail_idx == 0) {
		train->position = car->position;
	}
	sem_dynamic_array_add(train->cars, car);
	return SEM_OK;
}

bool sem_train_occupies(sem_train* train, sem_coordinate* tile) {
	for (uint32_t i=0; i < train->cars->tail_idx; i++) {
		sem_car* car = (sem_car*) train->cars->items[i];
		if (sem_coordinate_equal(car->position, tile)) return true;
	}

	return false;
}

void sem_train_destroy(sem_train* train) {
	sem_dynamic_array_destroy(train->cars);
	free(train->cars);
}	

void train_move_trailing(sem_dynamic_array* cars) {
	for (uint32_t i=cars->tail_idx-1; i > 0; i--) {
		sem_car* car_behind = (sem_car*) cars->items[i];
		sem_car* car_in_front = (sem_car*) cars->items[i-1];
		car_behind->position->x = car_in_front->position->x;
		car_behind->position->y = car_in_front->position->y;
		car_behind->track = car_in_front->track;
	}
}

sem_train* train_detect_collision(sem_train* t1) {
	sem_dynamic_array* trains = t1->world->trains;
	for (uint32_t t=0; t < trains->tail_idx; t++) {
		sem_train* t2 = trains->items[t];
		if (t2 == t1) continue;

		for (uint32_t c1=0; c1 < t1->cars->tail_idx; c1++) {
			sem_car* car1 = (sem_car*) t1->cars->items[c1];
			for (uint32_t c2=0; c2 < t2->cars->tail_idx; c2++) {
				sem_car* car2 = (sem_car*) t2->cars->items[c2];
				if (sem_coordinate_equal(car1->position, car2->position)) return t2;	
			}
		}
	}

	return NULL;
}
