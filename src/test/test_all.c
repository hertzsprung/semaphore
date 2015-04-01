#include <inttypes.h>

#include <glib.h>

#include "test_action_list.h"
#include "test_compass.h"
#include "test_dynamic_array.h"
#include "test_heap.h"
#include "test_input.h"
#include "test_serialize.h"
#include "test_timer.h"
#include "test_parser.h"
#include "test_track_cache.h"
#include "test_train.h"
#include "test_world.h"

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	add_tests_action_list();
	add_tests_compass();
	add_tests_dynamic_array();
	add_tests_heap();
	add_tests_input();
	add_tests_serialize();
	add_tests_timer();
	add_tests_parser();
	add_tests_track_cache();
	add_tests_train();
	add_tests_world();

	return g_test_run();
}

