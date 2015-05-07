#include <glib.h>
#include <stdio.h>

#include "test_serialize.h"
#include "sem_serialize.h"
#include "sem_world.h"

FILE* save_and_load(char* filename, sem_world* world);
void test_serialize_save_load_blank_tiles(void);
void test_serialize_load_track_tile(void);
void test_serialize_load_train(void);
void test_serialize_save_load_timer(void);

void add_tests_serialize() {
	g_test_add_func("/serialize/save_load_blank_tiles", test_serialize_save_load_blank_tiles);
	g_test_add_func("/serialize/load_track_tile", test_serialize_load_track_tile);
	g_test_add_func("/serialize/load_train", test_serialize_load_train);
	g_test_add_func("/serialize/save_load_timer", test_serialize_save_load_timer);
}

FILE* save_and_load(char* filename, sem_world* world) {
	FILE* file = fopen(filename, "w");
	sem_serialize_save(file, world);
	sem_world_destroy(world);
	fclose(file);

	file = fopen(filename, "r");
	sem_serialize_load(file, world);
	return file;
}

void test_serialize_save_load_blank_tiles() {
	sem_world world;
	world.max_x = 2;
	world.max_y = 1;
	sem_world_init_blank(&world);

	FILE* file = save_and_load("build/test/blank_tiles", &world);
	
	g_assert_cmpuint(world.max_x, ==, 2);
	g_assert_cmpuint(world.max_y, ==, 1);

	sem_world_destroy(&world);
	fclose(file);
}

void test_serialize_load_track_tile() {
	sem_world world;
	world.max_x = 1;
	world.max_y = 1;
	sem_world_init_blank(&world);

	sem_tile* tile = sem_tile_at(&world, 0, 0);
	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);
	sem_tile_set_track(tile, &track);

	FILE* file = save_and_load("build/test/track_tile", &world);

	tile = sem_tile_at(&world, 0, 0);
	g_assert_true(tile->class == TRACK);
	g_assert_true(tile->track->start == SEM_WEST);
	g_assert_true(tile->track->end == SEM_EAST);

	sem_world_destroy(&world);
	fclose(file);
}

void test_serialize_load_train() {
	FILE* file = fopen("src/test/load_train", "r");

	sem_world world;
	sem_serialize_load(file, &world);

	g_assert_cmpuint(world.trains->tail_idx, ==, 1);

	sem_train* train = world.trains->items[0];
	g_assert_true(train->state == STOPPED);
	g_assert_true(train->direction == SEM_EAST);

	sem_car* car = train->head_car;
	g_assert_cmpuint(car->position.x, ==, 1);
	g_assert_cmpuint(car->position.y, ==, 1);

	sem_track* track = car->track;
	g_assert_true(track->start == SEM_EAST);
	g_assert_true(track->end == SEM_WEST);

	g_assert_true(car->next != NULL);

	sem_world_destroy(&world);
	fclose(file);
}

void test_serialize_save_load_timer() {
	sem_world world;
	world.max_x = 1;
	world.max_y = 1;
	sem_world_init_blank(&world);
	world.timer->now = 123;
	world.timer->multiplier = 1.6;

	FILE* file = save_and_load("build/test/timer", &world);

	g_assert_cmpuint(world.timer->now, ==, 123);
	g_assert_cmpfloat(world.timer->multiplier, >, 1.59);
	g_assert_cmpfloat(world.timer->multiplier, <, 1.61);

	sem_world_destroy(&world);
	fclose(file);
}
