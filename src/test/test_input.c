#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>

#include "test_input.h"

#include "sem_action_list.h"
#include "sem_compass.h"
#include "sem_input.h"
#include "sem_dynamic_array.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

typedef struct {
	sem_dynamic_array heap;
	sem_world world;
	sem_train train;
} test_input_context;

void test_input_null_action_for_unoccupied_coordinate(test_input_context* test_ctx, const void* data);
void test_input_toggles_train_state(test_input_context* test_ctx, const void* data);
void test_input_null_action_on_blank_tile(test_input_context* test_ctx, const void* data);
void test_input_switches_points(test_input_context* test_ctx, const void* data);

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data));
void test_input_setup(test_input_context* test_ctx, const void* data);
void test_input_teardown(test_input_context* test_ctx, const void* data);

void add_tests_input(void) {
	add_test_input("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_input("/input/toggles_train_state", test_input_toggles_train_state);
	add_test_input("/input/null_action_on_blank_tile", test_input_null_action_on_blank_tile);
	add_test_input("/input/switches_points", test_input_switches_points);
}

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data)) {
	g_test_add(test_name, test_input_context, NULL, test_input_setup, test, test_input_teardown);
}

void test_input_setup(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->world.max_x = 2;
	test_ctx->world.max_y = 1;
	sem_world_init_blank(&(test_ctx->world));

	sem_train_init(&(test_ctx->train));
	sem_world_add_train(&(test_ctx->world), &(test_ctx->train));

	sem_heap_init(&(test_ctx->heap));
}

void test_input_teardown(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->world));
	sem_train_destroy(&(test_ctx->train));
	sem_dynamic_array_destroy(&(test_ctx->heap));
}

void test_input_null_action_for_unoccupied_coordinate(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);

	sem_coordinate position;
	sem_coordinate_set(&position, 1, 4);
	sem_car car;
	car.position = &position;
	sem_train_add_car(train, &car);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 3, 2);
	sem_input_event input;
	input.tile = &coord;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, world, &action);

	g_assert_null(action);
}

void test_input_toggles_train_state(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_coordinate position;
	sem_coordinate_set(&position, 0, 0);
	sem_car car;
	car.position = &position;

	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;
	train->state = STOPPED;

	// TODO: should really define the track here

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, world, &action);
	action->function(heap, action);

	action = sem_heap_remove_earliest(heap);
	g_assert_cmpuint(action->time, ==, 3000L);

	action->function(heap, action);
	free(action);

	g_assert_true(train->state == MOVING);
	g_assert_true(train->position->x == 1);
	g_assert_true(train->position->y == 0);
}

void test_input_null_action_on_blank_tile(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = BLANK;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, world, &action);
	g_assert_null(action);
}

void test_input_switches_points(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track trackW_NE;
	trackW_NE.start = SEM_WEST;
	trackW_NE.end = SEM_NORTH | SEM_EAST;

	sem_track trackW_E;
	trackW_E.start = SEM_WEST;
	trackW_E.end = SEM_EAST;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	tile->class = POINTS;
	tile->track = &trackW_NE;
	tile->points[0] = &trackW_E;
	tile->points[1] = &trackW_NE;
	tile->points[2] = NULL;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, world, &action);
	action->function(heap, action);
	free(action);

	g_assert_true(tile->track == &trackW_E);
}
