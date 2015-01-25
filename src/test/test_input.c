#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>

#include "test_input.h"

#include "sem_world.h"
#include "sem_train.h"
#include "sem_input.h"
#include "sem_heap.h"

void test_input_null_action_for_unoccupied_coordinate(void);
void test_input_toggles_train_state(void);

void add_tests_input(void) {
	g_test_add_func("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	g_test_add_func("/input/toggles_train_state", test_input_toggles_train_state);
}

void test_input_null_action_for_unoccupied_coordinate() {
	sem_train train;
	train.x = 1;
	train.y = 4;

	sem_world world;
	world.train = &train;

	sem_input_event input;
	input.x = 3;
	input.y = 2;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, &world, &action);

	g_assert_null(action);
}

void test_input_toggles_train_state() {
	sem_train train;
	train.x = 1;
	train.y = 4;
	train.moving = false;

	sem_world world;
	world.train = &train;

	sem_input_event input;
	input.x = 1;
	input.y = 4;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, &world, &action);
	action->function(NULL, action);

	free(action);
	g_assert_true(train.moving == true);
}
