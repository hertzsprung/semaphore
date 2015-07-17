#include <inttypes.h>

#include <glib.h>

#include "test_action_list.h"
#include "test_compass.h"
#include "test_dynamic_array.h"
#include "test_heap.h"
#include "test_input.h"
#include "test_parser.h"
#include "test_portal.h"
#include "test_serialize.h"
#include "test_signal.h"
#include "test_timer.h"
#include "test_track_cache.h"
#include "test_train.h"
#include "test_world.h"

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	add_tests_portal();
	return g_test_run();
}

