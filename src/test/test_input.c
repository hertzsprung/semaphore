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

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data));
void test_input_setup(test_input_context* test_ctx, const void* data);
void test_input_teardown(test_input_context* test_ctx, const void* data);

void add_tests_input(void) {
	add_test_input("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_input("/input/toggles_train_state", test_input_toggles_train_state);
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
	test_ctx->train.world = &(test_ctx->world);
	sem_dynamic_array_add(test_ctx->world.trains, &(test_ctx->train));

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
	sem_train_add_car(train, &position);

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
	sem_train_add_car(train, &position);
	train->direction = SEM_EAST;
	train->moving = false;

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

	g_assert_true(train->moving == true);
	g_assert_true(train->position->x == 1);
	g_assert_true(train->position->y == 0);
}
