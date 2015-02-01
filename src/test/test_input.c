#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>

#include "test_input.h"
#include "test_heap.h"

#include "sem_action_list.h"
#include "sem_compass.h"
#include "sem_input.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

void test_input_null_action_for_unoccupied_coordinate(void);
void test_input_toggles_train_state(sem_heap* heap, const void* data);

void add_tests_input(void) {
	g_test_add_func("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_heap("/input/toggles_train_state", test_input_toggles_train_state);
}

// TODO: use glib to handle train init/destroy
void test_input_null_action_for_unoccupied_coordinate() {
	sem_train train;
	sem_train_init(&train);

	sem_coordinate position;
	sem_coordinate_set(&position, 1, 4);
	sem_train_add_car(&train, &position);

	sem_world world;
	world.train = &train;

	sem_input_event input;
	input.x = 3;
	input.y = 2;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, &world, &action);

	g_assert_null(action);
}

// TODO: get glib to init/destroy the world and train!
void test_input_toggles_train_state(sem_heap* heap, const void* data) {
	#pragma unused(data)

	sem_train train;
	sem_train_init(&train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(&train, &position);
	train.direction = SEM_EAST;
	train.moving = false;

	sem_world world;
	world.max_x = 2;
	world.max_y = 1;
	sem_world_init_blank(&world);
	// TODO: should really define the track here

	world.train = &train;
	train.world = &world;

	sem_input_event input;
	input.time = 3000L;
	input.x = 0;
	input.y = 0;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, &world, &action);
	action->function(heap, action);

	action = sem_heap_remove_earliest(heap);
	g_assert_cmpuint(action->time, ==, 3000L);

	action->function(heap, action);
	free(action);

	g_assert_true(train.moving == true);
	g_assert_true(train.position->x == 1);
	g_assert_true(train.position->y == 0);
}
