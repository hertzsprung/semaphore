#include <glib.h>
#include <stdio.h>

#include "test_serialize.h"
#include "sem_serialize.h"
#include "sem_world.h"

void test_serialize_load_blank_tiles(void);

void add_tests_serialize(void) {
	g_test_add_func("/serialize/load_blank_tiles", test_serialize_load_blank_tiles);
}

void test_serialize_load_blank_tiles() {
	FILE* file = fopen("src/test/load_blank_tile", "r");
	sem_world world;
	sem_serialize_load(file, &world);
	
	g_assert_cmpuint(world.max_x, ==, 2);
	g_assert_cmpuint(world.max_y, ==, 1);

	fclose(file);
}
