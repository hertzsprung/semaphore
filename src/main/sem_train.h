#ifndef SEM_TRAIN_H
#define SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>
#include <uuid/uuid.h>

typedef struct sem_train sem_train;
typedef struct sem_car sem_car;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

typedef enum {
	STOPPED, MOVING, CRASHED, DERAILED
} sem_train_state;

typedef enum {
	ENTERING, ENTERED
} sem_train_portal_state;

struct sem_car {
	sem_coordinate position;
	sem_track* track;
	sem_car* previous;
	sem_car* next;
};

struct sem_train {
	sem_world* world;
	uuid_t id;
	char* name;
	sem_train_state state;
	sem_train_portal_state portal_state;
	sem_coordinate* position; /**< position of the train's head car */
	sem_coordinate entry_position;
	uint8_t spawn_cars_remaining;
	uint8_t cars;
	sem_car* head_car;
	sem_car* tail_car;
	unit_vector direction;
};

typedef struct {
	bool stopped_at_buffer;
} sem_train_move_outcome;

sem_success sem_train_init(sem_train* train);

sem_success sem_train_move(sem_train* train, sem_train_move_outcome* outcome);

void sem_train_reverse(sem_train* train);

void sem_train_add_car(sem_train* train, sem_car* car);

bool sem_train_occupies(sem_train* train, sem_coordinate* tile);

void sem_train_destroy(sem_train* train);

#endif
