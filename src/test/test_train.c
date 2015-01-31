#include <glib.h>

#include "test_train.h"

#include "sem_compass.h"
#include "sem_train.h"
#include "sem_world.h"

void test_train_moves_given_velocity(void);
void test_train_error_moves_onto_blank_tile(void);
void test_train_follows_track(void);

void add_tests_train() {
//	g_test_add_func("/train/moves_given_velocity", test_train_moves_given_velocity);
	g_test_add_func("/train/error_moves_onto_blank_tile", test_train_error_moves_onto_blank_tile);
	g_test_add_func("/train/train_follows_track", test_train_follows_track);
}

void test_train_moves_given_velocity() {
	sem_train train;
	train.x = 3;
	train.y = 1;
	train.direction = SEM_NORTH | SEM_EAST;

	sem_train_move(&train);

	g_assert_cmpuint(train.x, ==, 4);
	g_assert_cmpuint(train.y, ==, 0);
}

void test_train_error_moves_onto_blank_tile() {
	sem_train train;
	train.x = 0;
	train.y = 0;
	train.direction = SEM_EAST;

	sem_world world;
	world.max_x = 2;
	world.max_y = 1;

	sem_world_init_blank(&world);

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(&world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	train.world = &world;

	g_assert_cmpint(sem_train_move(&train), ==, SEM_ERROR);
}

// TODO: get glib to handle creation/destruction of world
void test_train_follows_track() {
	sem_train train;
	train.x = 0;
	train.y = 0;
	train.direction = SEM_EAST;

	sem_world world;
	world.max_x = 2;
	world.max_y = 1;

	sem_world_init_blank(&world);

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(&world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	sem_track track_SE_W;
	track_SE_W.start = SEM_SOUTH | SEM_EAST;
	track_SE_W.end = SEM_WEST;

	tile = sem_tile_at(&world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SE_W;

	train.world = &world;
	
	sem_train_move(&train);

	g_assert_true(train.direction == (SEM_SOUTH | SEM_EAST));
}
