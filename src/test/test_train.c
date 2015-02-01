#include <glib.h>

#include "test_train.h"

#include "sem_compass.h"
#include "sem_train.h"
#include "sem_world.h"

void test_train_moves_given_velocity(sem_world* world, const void* data);
void test_train_error_moves_onto_blank_tile(sem_world* world, const void* data);
void test_train_follows_track(sem_world* world, const void* data);
void test_train_moves_head_car(sem_world* world, const void* data);
void test_train_moves_trailing_cars(sem_world* world, const void* data);

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data));
void test_world_setup(sem_world* world, const void* data);
void test_world_teardown(sem_world* world, const void* data);

void add_tests_train() {
	add_test_world("/train/moves_given_velocity", test_train_moves_given_velocity);
	add_test_world("/train/error_moves_onto_blank_tile", test_train_error_moves_onto_blank_tile);
	add_test_world("/train/train_follows_track", test_train_follows_track);
	add_test_world("/train/moves_head_car", test_train_moves_head_car);
	add_test_world("/train/moves_trailing_cars", test_train_moves_trailing_cars);
}

void add_test_world(const char *test_name, void (*test)(sem_world*, const void* data)) {
	g_test_add(test_name, sem_world, NULL, test_world_setup, test, test_world_teardown);
}

void test_world_setup(sem_world* world, const void* data) {
	#pragma unused(data)

	world->max_x = 4;
	world->max_y = 4;
	sem_world_init_blank(world);
}

void test_world_teardown(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_world_destroy(world);
}

// TODO: get glib to handle train init/destroy
void test_train_moves_given_velocity(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	sem_train_init(&train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 1);
	sem_train_add_car(&train, &position);

	train.direction = SEM_NORTH | SEM_EAST;

	sem_track track_SW_NE;
	track_SW_NE.start = SEM_SOUTH | SEM_WEST;
	track_SW_NE.end = SEM_NORTH | SEM_EAST;

	sem_tile* tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SW_NE;

	train.world = world;

	sem_train_move(&train);

	g_assert_cmpuint(train.position->x, ==, 1);
	g_assert_cmpuint(train.position->y, ==, 0);
}

void test_train_error_moves_onto_blank_tile(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	sem_train_init(&train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(&train, &position);
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
	sem_train_init(&train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(&train, &position);
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

void test_train_moves_head_car(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	sem_train_init(&train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 1, 0);
	sem_train_add_car(&train, &head_car);

	sem_coordinate car2;
	sem_coordinate_set(&car2, 0, 0);
	sem_train_add_car(&train, &car2);

	train.direction = SEM_EAST;

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 2, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	train.world = world;

	sem_train_move(&train);

	g_assert_cmpuint(train.position->x, ==, 2);
	g_assert_cmpuint(train.position->y, ==, 0);
}

// TODO: lots of C&P from test_train_moves_head_car
void test_train_moves_trailing_cars(sem_world* world, const void* data) {
	#pragma unused(data)

	sem_train train;
	sem_train_init(&train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 2, 0);
	sem_train_add_car(&train, &head_car);

	sem_coordinate car2;
	sem_coordinate_set(&car2, 1, 0);
	sem_train_add_car(&train, &car2);

	sem_coordinate car3;
	sem_coordinate_set(&car3, 0, 0);
	sem_train_add_car(&train, &car3);

	train.direction = SEM_EAST;

	sem_track track_E_W;
	track_E_W.start = SEM_EAST;
	track_E_W.end = SEM_WEST;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 2, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 3, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	train.world = world;

	sem_train_move(&train);

	g_assert_cmpuint(car2.x, ==, 2);
	g_assert_cmpuint(car2.y, ==, 0);

	g_assert_cmpuint(car3.x, ==, 1);
	g_assert_cmpuint(car3.y, ==, 0);
}
