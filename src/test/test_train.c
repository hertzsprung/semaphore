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
void test_train_car_occupies_track(test_train_context* test_ctx, const void* data);
void test_train_moves_trailing_car_onto_track(test_train_context* test_ctx, const void* data);
void test_train_derails_when_need_points_switch(test_train_context* test_ctx, const void* data);

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
	add_test_train("/train/car_occupies_track", test_train_car_occupies_track);
	add_test_train("/train/moves_trailing_car_onto_track", test_train_moves_trailing_car_onto_track);
	add_test_train("/train/derails_when_need_points_switch", test_train_derails_when_need_points_switch);
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
}

void test_train_moves_given_velocity(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 1);
	sem_train_add_car(train, &car);

	train->direction = SEM_NORTH | SEM_EAST;

	sem_track track_SW_NE;
	sem_track_set(&track_SW_NE, SEM_SOUTH | SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_SW_NE);

	sem_train_move(train);

	g_assert_cmpuint(train->position->x, ==, 1);
	g_assert_cmpuint(train->position->y, ==, 0);
}

void test_train_error_moves_onto_blank_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	
	g_assert_cmpint(sem_train_move(train), ==, SEM_ERROR);
}

void test_train_follows_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_track track_SE_W;
	sem_track_set(&track_SE_W, SEM_WEST, SEM_SOUTH | SEM_EAST);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_SE_W);

	sem_train_move(train);

	g_assert_true(train->direction == (SEM_SOUTH | SEM_EAST));
}

void test_train_follows_secondary_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_track track_SE_W;
	sem_track_set(&track_SE_W, SEM_SOUTH | SEM_EAST, SEM_WEST);

	sem_track track_N_S_SE_W;
	sem_track_set(&track_N_S_SE_W, SEM_NORTH, SEM_SOUTH);
	track_N_S_SE_W.next = &track_SE_W;

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_N_S_SE_W);

	g_assert_true(sem_train_move(train) == SEM_OK);
	g_assert_true(train->direction == (SEM_SOUTH | SEM_EAST));
}

void test_train_moves_head_car(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car head_car;
	sem_coordinate_set(&(head_car.position), 1, 0);
	sem_train_add_car(train, &head_car);

	sem_car car2;
	sem_coordinate_set(&(car2.position), 0, 0);
	sem_train_add_car(train, &car2);

	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 2, 0), &track_E_W);
	
	sem_train_move(train);

	g_assert_cmpuint(train->position->x, ==, 2);
	g_assert_cmpuint(train->position->y, ==, 0);
}

// TODO: lots of C&P from test_train_moves_head_car
void test_train_moves_trailing_cars(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_car head_car;
	sem_coordinate_set(&(head_car.position), 2, 0);
	sem_train_add_car(train, &head_car);

	sem_car car2;
	sem_coordinate_set(&(car2.position), 1, 0);
	sem_train_add_car(train, &car2);

	sem_car car3;
	sem_coordinate_set(&(car3.position), 0, 0);
	sem_train_add_car(train, &car3);

	train->direction = SEM_EAST;

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 2, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 3, 0), &track_E_W);
	
	sem_train_move(train);

	g_assert_cmpuint(car2.position.x, ==, 2);
	g_assert_cmpuint(car2.position.y, ==, 0);

	g_assert_cmpuint(car3.position.x, ==, 1);
	g_assert_cmpuint(car3.position.y, ==, 0);
}

void test_train_head_car_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_position;
	sem_coordinate_set(&head_position, 2, 0);
	sem_car head_car;
	head_car.position = head_position;
	sem_train_add_car(train, &head_car);

	g_assert_true(sem_train_occupies(train, &head_position));
}

void test_train_second_car_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_car head_car;
	sem_coordinate_set(&(head_car.position), 2, 0);
	sem_train_add_car(train, &head_car);

	sem_coordinate second_car_position;
	sem_coordinate_set(&second_car_position, 1, 0);
	sem_car second_car;
	second_car.position = second_car_position;
	sem_train_add_car(train, &second_car);

	g_assert_true(sem_train_occupies(train, &second_car_position));
}

void test_train_not_occupies_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);

	sem_coordinate head_position;
	sem_coordinate_set(&head_position, 2, 0);
	sem_car head_car;
	head_car.position = head_position;
	sem_train_add_car(train, &head_car);

	sem_coordinate other_tile;
	sem_coordinate_set(&other_tile, 1, 1);

	g_assert_false(sem_train_occupies(train, &other_tile));
}

void test_train_crashes_by_occupying_same_tile(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train1 = &(test_ctx->train1);
	sem_train* train2 = &(test_ctx->train2);
	sem_world* world = &(test_ctx->world);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	sem_tile_set_track(tile, &trackW_E);

	train1->state = MOVING;
	train1->direction = SEM_EAST;

	sem_car train1_car;
	sem_coordinate_set(&(train1_car.position), 0, 0);
	sem_train_add_car(train1, &train1_car);

	train2->state = STOPPED;

	sem_car train2_car1;
	sem_coordinate_set(&(train2_car1.position), 1, 0);
	sem_train_add_car(train2, &train2_car1);

	sem_car train2_car2;
	sem_coordinate_set(&(train2_car2.position), 2, 0);
	sem_train_add_car(train2, &train2_car2);

	sem_train_move(train1); // move head of train1 into tail of train2

	g_assert_true(train1->state == CRASHED);
	g_assert_true(train2->state == CRASHED);
}

void test_train_car_occupies_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_track track_W_S;
	sem_track_set(&track_W_S, SEM_WEST, SEM_SOUTH);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_W_S);

	train->state = MOVING;
	train->direction = SEM_EAST;

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = &track_E_W;
	sem_train_add_car(train, &car);

	sem_train_move(train);

	g_assert_true(train->head_car->track == &track_W_S);
}

void test_train_moves_trailing_car_onto_track(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world* world = &(test_ctx->world);
	sem_train* train = &(test_ctx->train);

	sem_track track_E_W;
	sem_track_set(&track_E_W, SEM_EAST, SEM_WEST);

	sem_track track_W_S;
	sem_track_set(&track_W_S, SEM_WEST, SEM_SOUTH);

	sem_track track_N_S;
	sem_track_set(&track_N_S, SEM_NORTH, SEM_SOUTH);

	sem_tile_set_track(sem_tile_at(world, 0, 0), &track_E_W);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &track_W_S);
	sem_tile_set_track(sem_tile_at(world, 1, 1), &track_N_S);

	train->state = MOVING;
	train->direction = SEM_SOUTH;

	sem_car car1;
	sem_coordinate_set(&(car1.position), 1, 0);
	car1.track = &track_W_S;

	sem_car car2;
	sem_coordinate_set(&(car2.position), 0, 0);
	car2.track = &track_E_W;

	sem_train_add_car(train, &car1);
	sem_train_add_car(train, &car2);

	sem_train_move(train);

	g_assert_true(train->head_car->track == &track_N_S);
	g_assert_true(train->head_car->next->track == &track_W_S);
}

void test_train_derails_when_need_points_switch(test_train_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);

	sem_tile_acceptance acceptance;
	sem_tile_acceptance_init(&acceptance);
	
	train->direction = SEM_EAST;

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);

	sem_train_add_car(train, &car);

	sem_track trackNW_E;
	sem_track_set(&trackNW_E, SEM_NORTH | SEM_WEST, SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	sem_tile_set_points(tile, &trackNW_E);
	tile->points[1] = &trackW_E;

	sem_train_move(train);

	g_assert_true(train->state == DERAILED);
}
