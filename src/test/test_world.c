#include <glib.h>

#include "test_world.h"

#include "sem_world.h"

void test_world_create_blank(sem_world* world, const void* data);

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
}

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data)) {
	g_test_add(test_name, sem_world, NULL, test_world_setup, test, test_world_teardown);
}

void test_world_create_blank(sem_world* world, const void* data) {
	#pragma unused(data)

	g_assert_true(sem_tile_at(world, 0, 0)->class == BLANK);
	g_assert_true(sem_tile_at(world, 1, 0)->class == BLANK);
}
