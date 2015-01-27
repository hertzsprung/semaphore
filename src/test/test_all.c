#include <inttypes.h>

#include <glib.h>

#include "test_action_list.h"
#include "test_heap.h"
#include "test_input.h"
#include "test_timer.h"
#include "test_train.h"
#include "test_world.h"

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	add_tests_action_list();
	add_tests_heap();
	add_tests_input();

	g_test_add_func("/timer/increments_by_one_second", test_timer_increments_by_one_second);
	g_test_add_func("/timer/increments_twice_clock_with_multiple_of_two", test_timer_increments_twice_clock_with_multiple_of_two);

	add_tests_train();
	add_tests_world();

	return g_test_run();
}

