#ifndef SEM_PORTAL_H
#define SEM_PORTAL_H

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"
#include "sem_world.h"

typedef struct {
	sem_world* world;
	sem_coordinate position;
	unit_vector direction;
	uint8_t cars;
} sem_train_entry_context;

sem_success sem_train_entry_action(sem_dynamic_array* heap, sem_action* action);

sem_success sem_portal_spawn_car(sem_train* train);

#endif
