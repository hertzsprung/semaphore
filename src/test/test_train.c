#include <glib.h>

#include "test_train.h"

#include "sem_compass.h"
#include "sem_train.h"
#include "sem_world.h"

typedef struct {
	sem_world world;
	sem_train train;
	sem_train train1, train2;
} test_train_context;

void test_train_moves_given_velocity(test_train_context* test_ctx, const void* data);
void test_train_error_moves_onto_blank_tile(test_train_context* test_ctx, const void* data);
void test_train_follows_track(test_train_context* test_ctx, const void* data);
void test_train_follows_secondary_track(test_train_context* test_ctx, const void* data);
void test_train_moves_head_car(test_train_context* test_ctx, const void* data);
void test_train_moves_trailing_cars(test_train_context* test_ctx, const void* data);
void test_train_head_car_occupies_tile(test_train_context* test_ctx, const void* data);
void test_train_second_car_occupies_tile(test_train_context* test_ctx, const void* data);
void test_train_not_occupies_tile(test_train_context* test_ctx, const void* data);
void test_train_crashes_by_occupying_same_tile(test_train_context* test_ctx, const void* data);

void add_test_train(const char *test_name, void (*test)(test_train_context*, const void* data));
void test_train_setup(test_train_context* test_ctx, const void* data);
void test_train_teardown(test_train_context* test_ctx, const void* data);

void add_tests_train() {
	add_test_train("/train/moves_given_velocity", test_train_moves_given_velocity);
	add_test_train("/train/error_moves_onto_blank_tile", test_train_error_moves_onto_blank_tile);
	add_test_train("/train/follows_track", test_train_follows_track);
	add_test_train("/train/follows_secondary_track", test_train_follows_secondary_track);
	add_test_train("/train/moves_head_car", test_train_moves_head_car);
	add_test_train("/train/moves_trailing_cars", test_train_moves_trailing_cars);
	add_test_train("/train/head_car_occupies_tile", test_train_head_car_occupies_tile);
	add_test_train("/train/second_car_occupies_tile", test_train_second_car_occupies_tile);
	add_test_train("/train/not_occupies_tile", test_train_not_occupies_tile);
	add_test_train("/train/crashes_by_occupying_same_tile", test_train_crashes_by_occupying_same_tile);
}

void add_test_train(const char *test_name, void (*test)(test_train_context*, const void* data)) {
	g_test_add(test_name, test_train_context, NULL, test_train_setup, test, test_train_teardown);
}

void test_train_setup(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->world.max_x = 4;
	test_ctx->world.max_y = 4;
	sem_world_init_blank(&(test_ctx->world));

	sem_train_init(&(test_ctx->train));
	sem_train_init(&(test_ctx->train1));
	sem_train_init(&(test_ctx->train2));

	sem_world_add_train(&(test_ctx->world), &(test_ctx->train));
	sem_world_add_train(&(test_ctx->world), &(test_ctx->train1));
	sem_world_add_train(&(test_ctx->world), &(test_ctx->train2));
}

void test_train_teardown(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->world));
	sem_train_destroy(&(test_ctx->train));
	sem_train_destroy(&(test_ctx->train1));
	sem_train_destroy(&(test_ctx->train2));
}

void test_train_moves_given_velocity(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 1);
	sem_train_add_car(train, &position);

	train->direction = SEM_NORTH | SEM_EAST;

	sem_track track_SW_NE;
	sem_track_set(&track_SW_NE, SEM_SOUTH | SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SW_NE;

	sem_train_move(train);

	g_assert_cmpuint(train->position->x, ==, 1);
	g_assert_cmpuint(train->position->y, ==, 0);
}

void test_train_error_moves_onto_blank_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(train, &position);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	g_assert_cmpint(sem_train_move(train), ==, SEM_ERROR);
}

void test_train_follows_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(train, &position);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	sem_track track_SE_W;
	sem_track_set(&track_SE_W, SEM_WEST, SEM_SOUTH | SEM_EAST);

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_SE_W;

	sem_train_move(train);

	g_assert_true(train->direction == (SEM_SOUTH | SEM_EAST));
}

void test_train_follows_secondary_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_train_add_car(train, &position);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_track track_SE_W;
	sem_track_set(&track_SE_W, SEM_SOUTH | SEM_EAST, SEM_WEST);

	sem_track track_N_S_SE_W;
	sem_track_set(&track_N_S_SE_W, SEM_NORTH, SEM_SOUTH);
	track_N_S_SE_W.next = &track_SE_W;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_N_S_SE_W;

	g_assert_true(sem_train_move(train) == SEM_OK);
	g_assert_true(train->direction == (SEM_SOUTH | SEM_EAST));
}

void test_train_moves_head_car(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 1, 0);
	sem_train_add_car(train, &head_car);

	sem_coordinate car2;
	sem_coordinate_set(&car2, 0, 0);
	sem_train_add_car(train, &car2);

	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 1, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;

	tile = sem_tile_at(world, 2, 0);
	tile->class = TRACK;
	tile->track = &track_E_W;
	
	sem_train_move(train);

	g_assert_cmpuint(train->position->x, ==, 2);
	g_assert_cmpuint(train->position->y, ==, 0);
}

// TODO: lots of C&P from test_train_moves_head_car
void test_train_moves_trailing_cars(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 2, 0);
	sem_train_add_car(train, &head_car);

	sem_coordinate car2;
	sem_coordinate_set(&car2, 1, 0);
	sem_train_add_car(train, &car2);

	sem_coordinate car3;
	sem_coordinate_set(&car3, 0, 0);
	sem_train_add_car(train, &car3);

	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

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
	
	sem_train_move(train);

	g_assert_cmpuint(car2.x, ==, 2);
	g_assert_cmpuint(car2.y, ==, 0);

	g_assert_cmpuint(car3.x, ==, 1);
	g_assert_cmpuint(car3.y, ==, 0);
}

void test_train_head_car_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 2, 0);
	sem_train_add_car(train, &head_car);

	g_assert_true(sem_train_occupies(train, &head_car));
}

void test_train_second_car_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 2, 0);
	sem_train_add_car(train, &head_car);

	sem_coordinate second_car;
	sem_coordinate_set(&second_car, 1, 0);
	sem_train_add_car(train, &second_car);

	g_assert_true(sem_train_occupies(train, &second_car));
}

void test_train_not_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_car;
	sem_coordinate_set(&head_car, 2, 0);
	sem_train_add_car(train, &head_car);

	sem_coordinate other_tile;
	sem_coordinate_set(&other_tile, 1, 1);

	g_assert_false(sem_train_occupies(train, &other_tile));
}

void test_train_crashes_by_occupying_same_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train1 = &(test_ctx->train1);
	train1->state = MOVING;
	train1->direction = SEM_EAST;

	sem_coordinate train1_car;
	sem_coordinate_set(&train1_car, 0, 0);
	sem_train_add_car(train1, &train1_car);

	sem_train* train2 = &(test_ctx->train2);
	train2->state = STOPPED;

	sem_coordinate train2_car1;
	sem_coordinate_set(&train2_car1, 1, 0);
	sem_train_add_car(train2, &train2_car1);

	sem_coordinate train2_car2;
	sem_coordinate_set(&train2_car2, 2, 0);
	sem_train_add_car(train2, &train2_car2);

	sem_train_move(train1); // move head of train1 into tail of train2

	g_assert_true(train1->state == CRASHED);
	g_assert_true(train2->state == CRASHED);
}
