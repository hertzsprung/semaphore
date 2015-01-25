#include <stddef.h>
#include <glib.h>

#include "test_input.h"

#include "sem_world.h"
#include "sem_train.h"
#include "sem_input.h"
#include "sem_heap.h"

void test_input_null_action_for_unoccupied_coordinate(void);

void add_tests_input(void) {
	g_test_add_func("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
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
