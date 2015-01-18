#include <inttypes.h>

#include <glib.h>

#include "test_action_list.h"
#include "test_heap.h"
#include "test_timer.h"

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	add_tests_action_list();
	add_tests_heap();

	g_test_add_func("/timer/increments_by_one_second", test_timer_increments_by_one_second);
	g_test_add_func("/timer/increments_twice_clock_with_multiple_of_two", test_timer_increments_twice_clock_with_multiple_of_two);

	return g_test_run();
}

