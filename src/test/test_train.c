#include <glib.h>

#include "test_train.h"

#include "sem_compass.h"
#include "sem_train.h"

void test_train_moves_given_velocity(void);

void add_tests_train() {
	g_test_add_func("/train/moves_given_velocity", test_train_moves_given_velocity);
}

void test_train_moves_given_velocity() {
	sem_train train;
	train.x = 3;
	train.y = 1;
	train.velocity = SEM_NORTH | SEM_EAST;

	sem_train_move(&train);

	g_assert_cmpuint(train.x, ==, 4);
	g_assert_cmpuint(train.y, ==, 0);
}
