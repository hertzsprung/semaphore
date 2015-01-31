#include <glib.h>

#include "test_train.h"

#include "sem_compass.h"
#include "sem_train.h"
#include "sem_world.h"

void test_train_moves_given_velocity(sem_world* world, const void* data);
void test_train_error_moves_onto_blank_tile(sem_world* world, const void* data);
void test_train_follows_track(sem_world* world, const void* data);

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data));
void test_world_setup(sem_world* world, const void* data);
void test_world_teardown(sem_world* world, const void* data);

void add_tests_train() {
	add_test_world("/train/moves_given_velocity", test_train_moves_given_velocity);
	add_test_world("/train/error_moves_onto_blank_tile", test_train_error_moves_onto_blank_tile);
	add_test_world("/train/train_follows_track", test_train_follows_track);
}

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data)) {
	g_test_add(test_name, sem_world, NULL, test_world_setup, test, test_world_teardown);
}

void test_world_setup(sem_world* world, const void* data) {
	#pragma unused(data)

	world->max_x = 2;
	world->max_y = 2;
	sem_world_init_blank(world);
}

void test_world_teardown(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_world_destroy(world);
}

void test_train_moves_given_velocity(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	train.x = 0;
	train.y = 1;
	train.direction = SEM_NORTH | SEM_EAST;

	sem_track track_SW_NE;
	track_SW_NE.start = SEM_SOUTH | SEM_WEST;
	track_SW_NE.end = SEM_NORTH | SEM_EAST;

	sem_tile* tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SW_NE;

	train.world = world;

	sem_train_move(&train);

	g_assert_cmpuint(train.x, ==, 1);
	g_assert_cmpuint(train.y, ==, 0);
}

void test_train_error_moves_onto_blank_tile(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	train.x = 0;
	train.y = 0;
	train.direction = SEM_EAST;

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	train.world = world;

	g_assert_cmpint(sem_train_move(&train), ==, SEM_ERROR);
}

void test_train_follows_track(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	train.x = 0;
	train.y = 0;
	train.direction = SEM_EAST;

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	sem_track track_SE_W;
	track_SE_W.start = SEM_WEST;
	track_SE_W.end = SEM_SOUTH | SEM_EAST;

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SE_W;

	train.world = world;
	
	sem_train_move(&train);

	g_assert_true(train.direction == (SEM_SOUTH | SEM_EAST));
}
