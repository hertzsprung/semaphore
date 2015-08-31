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

void sem_train_move_outcome_init(sem_train_move_outcome* outcome);
void train_move_trailing(sem_car* tail_car);
sem_train* train_detect_collision(sem_train* train);
void reverse_cars(sem_car* car);
unit_vector train_tail_end(sem_train* train);

sem_success sem_train_init(sem_train* train) {
	uuid_generate(train->id);
	train->name = strdup("<none>");
	train->state = STOPPED;
	train->portal_state = ENTERED;
	train->direction = 0;
	train->cars = 0;
	train->head_car = NULL;
	train->tail_car = NULL;
	train->headless = false;
	train->spawn_cars_remaining = 0;
	train->exit_position = NULL;
	return SEM_OK;
}

sem_success sem_train_move(sem_train* train, sem_train_move_outcome* outcome) {
	sem_train_move_outcome_init(outcome);
	sem_coordinate new_position;
	new_position.x = train->position->x + SEM_COMPASS_X(train->direction);
	new_position.y = train->position->y + SEM_COMPASS_Y(train->direction);

	sem_tile* tile = sem_tile_at_coord(train->world, &new_position);
	sem_tile_acceptance acceptance;
	if (sem_tile_accept(train, tile, &acceptance) != SEM_OK) return SEM_ERROR;

	if (acceptance.reached_buffer) {
		train->state = STOPPED;
		outcome->stopped_at_buffer = true;
	} else if (acceptance.signalling.stop) {
		train->state = STOPPED;
	} else {
		train_move_trailing(train->tail_car);
		train->position->x = new_position.x;
		train->position->y = new_position.y;
		train->direction = acceptance.direction;
		train->head_car->track = acceptance.track;	
		if (acceptance.need_points_switch) train->state = DERAILED;
		if (acceptance.exiting) train->portal_state = EXITING;

		sem_train* collided_train = train_detect_collision(train);
		if (collided_train != NULL) {
			train->state = CRASHED;
			collided_train->state = CRASHED;
		}
	}

	return SEM_OK;
}

void sem_train_move_outcome_init(sem_train_move_outcome* outcome) {
	outcome->stopped_at_buffer = false;
}

void sem_train_reverse(sem_train* train) {
	train->direction = train_tail_end(train);
	train->position = &(train->tail_car->position);

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
	unit_vector tail_end_of_car = 0;

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

void sem_train_add_car(sem_train* train, sem_car* car) {
	train->cars++;
	if (train->head_car == NULL) {
		train->position = &(car->position);
	}
	if (train->tail_car != NULL) train->tail_car->next = car;
	car->previous = train->tail_car;
	car->next = NULL;
	if (train->head_car == NULL) train->head_car = car;
	train->tail_car = car;
}

void sem_train_remove_head_car(sem_train* train) {
	if (train->head_car == NULL) return;
	train->cars--;
	sem_car* old_head = train->head_car;
	free(old_head);
	train->head_car = train->head_car->next;
	train->head_car->previous = NULL;
	train->position = &(train->head_car->position);
	train->headless = true;
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
	// FIXME: free cars
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
