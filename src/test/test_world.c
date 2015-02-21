#include <glib.h>

#include "test_world.h"

#include "sem_world.h"

void test_world_create_blank(sem_world* world, const void* data);
void test_world_switch_points_to_next_in_array(sem_world* world, const void* data);

void test_world_setup(sem_world* world, const void* data);
void test_world_teardown(sem_world* world, const void* data);
void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data));

void test_world_setup(sem_world* world, const void* data) {
	#pragma unused(data)

	world->max_x = 2;
	world->max_y = 1;

	sem_world_init_blank(world);
}

void test_world_teardown(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_world_destroy(world);
}

void add_tests_world() {
	add_test_world("/world/create_blank", test_world_create_blank);
	add_test_world("/world/switch_points_to_next_in_array", test_world_switch_points_to_next_in_array);
}

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data)) {
	g_test_add(test_name, sem_world, NULL, test_world_setup, test, test_world_teardown);
}

void test_world_create_blank(sem_world* world, const void* data) {
	#pragma unused(data)

	g_assert_true(sem_tile_at(world, 0, 0)->class == BLANK);
	g_assert_true(sem_tile_at(world, 1, 0)->class == BLANK);
}

void test_world_switch_points_to_next_in_array(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_track trackW_NE;
	sem_track trackW_SE;
	sem_track trackW_E;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = POINTS;
	tile->track = &trackW_NE;
	tile->points[0] = &trackW_SE;
	tile->points[1] = &trackW_E;
	tile->points[2] = &trackW_NE;

	sem_tile_switch_points(tile);

	g_assert_true(tile->track == &trackW_SE);
}
