#ifndef SEM_TRAIN_H
#define SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>
#include <uuid/uuid.h>

typedef struct sem_train sem_train;
typedef struct sem_car sem_car;

typedef enum {
	FAST, MEDIUM, SLOW
} sem_train_speed;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_world.h"

typedef enum {
	STOPPED, MOVING, CRASHED, DERAILED
} sem_train_state;

typedef enum {
	ENTERING, ENTERED, EXITING
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
	sem_train_speed speed;
	uint32_t speeds[3];
	sem_train_state state;
	sem_train_portal_state portal_state;
	sem_coordinate* position; /**< position of the train's head car */
	sem_coordinate entry_position; /**< the position of the entry portal whence the train originated */
	bool has_exit_position;
	sem_coordinate exit_position; /**< the desired exit destination */
	uint8_t spawn_cars_remaining;
	bool headless; /**< true when the head of the train has already left through an exit portal */
	uint8_t cars;
	sem_car* head_car;
	sem_car* tail_car;
	unit_vector direction;
	sem_signal* signal;
	sem_signal* previous_signal;
	sem_signal* main_signal;
	sem_signal* previous_main_signal;
};

typedef struct {
	bool stopped_at_buffer;
} sem_train_move_outcome;

sem_success sem_train_init(sem_train* train);

sem_success sem_train_move(sem_train* train, sem_train_move_outcome* outcome);

void sem_train_reverse(sem_train* train);

void sem_train_add_car(sem_train* train, sem_car* car);

void sem_train_remove_head_car(sem_train* train);

bool sem_train_occupies(sem_train* train, sem_coordinate* tile);

void sem_train_destroy(sem_train* train);

#endif
