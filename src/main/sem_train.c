#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <uuid/uuid.h>

#include "sem_train.h"

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

void train_move_trailing(sem_car* tail_car);
sem_train* train_detect_collision(sem_train* train);
void reverse_cars(sem_car* car);
unit_vector train_tail_end(sem_train* train);

sem_success sem_train_init(sem_train* train) {
	uuid_generate(train->id);
	train->name = strdup("<none>");
	train->state = STOPPED;
	train->direction = 0;
	train->cars = 0;
	train->head_car = NULL;
	train->tail_car = NULL;
	return SEM_OK;
}

sem_success sem_train_move(sem_train* train) {
	train_move_trailing(train->tail_car);
	train->position->x += SEM_COMPASS_X(train->direction);
	train->position->y += SEM_COMPASS_Y(train->direction);

	sem_tile* tile = sem_tile_at_coord(train->world, train->position);
	sem_tile_acceptance acceptance;
	sem_success success = sem_tile_accept(train, tile, &acceptance);
	if (success != SEM_OK) return success;

	train->direction = acceptance.direction;
	train->head_car->track = acceptance.track;	
	if (acceptance.need_points_switch) train->state = DERAILED;

	sem_train* collided_train = train_detect_collision(train);
	if (collided_train != NULL) {
		train->state = CRASHED;
		collided_train->state = CRASHED;
	}

	return SEM_OK;
}

void sem_train_reverse(sem_train* train) {
	train->direction = train_tail_end(train);

	reverse_cars(train->head_car);

	sem_car* tmp = train->head_car;
	train->head_car = train->tail_car;
	train->tail_car = tmp;
}

void reverse_cars(sem_car* car) {
	if (car->next != NULL) reverse_cars(car->next);	

	sem_car* tmp = car->previous;
	car->previous = car->next;
	car->next = tmp;
}

unit_vector train_tail_end(sem_train* train) {
	sem_car* car = train->head_car;	
	unit_vector direction = train->direction;
	unit_vector tail_end_of_car;

	while (car != NULL) {
		if (direction == car->track->start) {
			tail_end_of_car = car->track->end;
		} else {
			tail_end_of_car = car->track->start;
		}
		direction = sem_compass_opposite_of(tail_end_of_car);
		car = car->next;
	}

	return sem_compass_opposite_of(direction);
}

sem_success sem_train_add_car(sem_train* train, sem_car* car) {
	train->cars++;
	if (train->head_car == 0) {
		train->position = &(car->position);
	}
	if (train->tail_car != NULL) train->tail_car->next = car;
	car->previous = train->tail_car;
	car->next = NULL;
	if (train->head_car == NULL) train->head_car = car;
	train->tail_car = car;
	return SEM_OK;
}

bool sem_train_occupies(sem_train* train, sem_coordinate* tile) {
	sem_car* car = train->head_car;
	while (car != NULL) {
		if (sem_coordinate_equal(car->position, *tile)) return true;
		car = car->next;
	}

	return false;
}

void sem_train_destroy(sem_train* train) {
	free(train->name);
}	

void train_move_trailing(sem_car* tail_car) {
	sem_car* car_behind = tail_car;
	while (car_behind != NULL && car_behind->previous != NULL) {
		sem_car* car_in_front = car_behind->previous;
		car_behind->position = car_in_front->position;
		car_behind->track = car_in_front->track;

		car_behind = car_in_front;
	}
}

sem_train* train_detect_collision(sem_train* t1) {
	sem_dynamic_array* trains = t1->world->trains;
	for (uint32_t t=0; t < trains->tail_idx; t++) {
		sem_train* t2 = trains->items[t];
		if (t2 == t1) continue;

		sem_car* car1 = t1->head_car;
		while (car1 != NULL) {
			sem_car* car2 = t2->head_car;
			while (car2 != NULL) {
				if (sem_coordinate_equal(car1->position, car2->position)) return t2;
				car2 = car2->next;
			}
			car1 = car1->next;
		}
	}

	return NULL;
}
