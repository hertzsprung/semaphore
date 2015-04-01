#include <glib.h>
#include <stdio.h>

#include "test_serialize.h"
#include "sem_serialize.h"
#include "sem_world.h"

void test_serialize_load_blank_tiles(void);
void test_serialize_load_track_tile(void);

void add_tests_serialize(void) {
	g_test_add_func("/serialize/load_blank_tiles", test_serialize_load_blank_tiles);
	g_test_add_func("/serialize/load_track_tile", test_serialize_load_track_tile);
}

void test_serialize_load_blank_tiles() {
	FILE* file = fopen("src/test/load_blank_tiles", "r");
	sem_world world;
	sem_serialize_load(file, &world);
	
	g_assert_cmpuint(world.max_x, ==, 2);
	g_assert_cmpuint(world.max_y, ==, 1);

	sem_world_destroy(&world);
	fclose(file);
}

void test_serialize_load_track_tile() {
	FILE* file = fopen("src/test/load_track_tile", "r");

	sem_world world;
	sem_serialize_load(file, &world);

	sem_tile* tile = sem_tile_at(&world, 0, 0);
	g_assert_true(tile->class == TRACK);
	g_assert_true(tile->track->start == SEM_WEST);
	g_assert_true(tile->track->end == SEM_EAST);

	sem_world_destroy(&world);
	fclose(file);
}
