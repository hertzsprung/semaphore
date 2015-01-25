#include <glib.h>

#include "test_world.h"

#include "sem_world.h"

void test_world_create_blank(void);

void add_tests_world() {
	g_test_add_func("/world/create_blank", test_world_create_blank);
}

void test_world_create_blank() {
	sem_world world;
	world.max_x = 2;
	world.max_y = 1;

	sem_world_init_blank(&world);

	g_assert_true(sem_tile_at(&world, 0, 0)->class == BLANK);
	g_assert_true(sem_tile_at(&world, 1, 0)->class == BLANK);

	sem_world_destroy(&world);
}
