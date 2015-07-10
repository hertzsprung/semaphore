#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>
#include <stdio.h>

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
	sem_train* train;
} test_input_context;

void test_input_null_action_for_unoccupied_coordinate(test_input_context* test_ctx, const void* data);
void test_input_toggles_train_state(test_input_context* test_ctx, const void* data);
void test_input_reverses_train(test_input_context* test_ctx, const void* data);
void test_input_removes_derailed_train(test_input_context* test_ctx, const void* data);
void test_input_unremoves_derailed_train_after_crash(test_input_context* test_ctx, const void* data);
void test_input_null_action_on_blank_tile(test_input_context* test_ctx, const void* data);
void test_input_switches_points(test_input_context* test_ctx, const void* data);
void test_input_reverses_at_buffer(test_input_context* test_ctx, const void* data);
void test_input_signal_green_to_red(test_input_context* test_ctx, const void* data);

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data));
void test_input_setup(test_input_context* test_ctx, const void* data);
void test_input_teardown(test_input_context* test_ctx, const void* data);

void add_tests_input(void) {
	add_test_input("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_input("/input/toggles_train_state", test_input_toggles_train_state);
	add_test_input("/input/reverses_train", test_input_reverses_train);
	add_test_input("/input/removes_derailed_train", test_input_removes_derailed_train);
	add_test_input("/input/unremoves_derailed_train_after_crash", test_input_unremoves_derailed_train_after_crash);
	add_test_input("/input/null_action_on_blank_tile", test_input_null_action_on_blank_tile);
	add_test_input("/input/switches_points", test_input_switches_points);
	add_test_input("/input/reverses_at_buffer", test_input_reverses_at_buffer);
	add_test_input("/input/signal_green_to_red", test_input_signal_green_to_red);
}

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data)) {
	g_test_add(test_name, test_input_context, NULL, test_input_setup, test, test_input_teardown);
}

void test_input_setup(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->world.max_x = 3;
	test_ctx->world.max_y = 1;
	sem_world_init_blank(&(test_ctx->world));

	test_ctx->train = malloc(sizeof(sem_train));
	sem_train_init(test_ctx->train);
	sem_world_add_train(&(test_ctx->world), test_ctx->train);

	sem_heap_init(&(test_ctx->heap));
}

void test_input_teardown(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->world));
	sem_dynamic_array_destroy(&(test_ctx->heap));
}

void test_input_null_action_for_unoccupied_coordinate(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);

	sem_car car;
	sem_coordinate_set(&(car.position), 1, 4);
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
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	sem_tile_set_track(sem_tile_at(world, 0, 0), &trackW_E);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &trackW_E);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);

	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;
	train->state = STOPPED;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;
	input.rank = PRIMARY;

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

void test_input_reverses_train(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = &track;

	sem_train_add_car(train, &car);
	train->direction = SEM_EAST;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;
	input.rank = SECONDARY;

	sem_action* action = NULL;
	sem_train_input_act_upon(&input, world, &action);

	g_assert_nonnull(action);
	action->function(heap, action);

	g_assert_true(train->direction == SEM_WEST);
}

void test_input_removes_derailed_train(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	sem_tile_set_track(tile, &trackW_E);

	tile = sem_tile_at(world, 1, 0); // TODO: this isn't being set properly
	sem_track trackNW_E;
	sem_track_set(&trackNW_E, SEM_NORTH | SEM_WEST, SEM_EAST);
	sem_tile_set_points(tile, &trackNW_E);
	tile->points[1] = &trackW_E;

	train->state = MOVING;
	train->direction = SEM_EAST;
	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = &trackW_E;
	sem_train_add_car(train, &car);

	sem_action action;
	action.time = 4000;
	action.context = train;
	g_assert_true(sem_move_train_action(heap, &action) == SEM_OK);

	g_assert_true(train->state == DERAILED);

	sem_action* remove_train_action = sem_heap_remove_earliest(heap);
	remove_train_action->function(heap, remove_train_action);

	g_assert_cmpuint(world->trains->tail_idx, ==, 0);
}

void test_input_unremoves_derailed_train_after_crash(test_input_context* test_ctx, const void* data) {
	#pragma unused(test_ctx)
	#pragma unused(data)
	// TODO
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
	sem_track_set(&trackW_NE, SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	sem_tile_set_points(tile, &trackW_NE);
	tile->points[0] = &trackW_E;
	tile->points[1] = &trackW_NE;

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

void test_input_reverses_at_buffer(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world* world = &(test_ctx->world);
	sem_train* train = test_ctx->train;
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	sem_tile_set_track(sem_tile_at(world, 0, 0), &trackW_E);
	sem_tile_set_track(sem_tile_at(world, 1, 0), &trackW_E);
	sem_tile_set_buffer(sem_tile_at(world, 2, 0), &trackW_E);

	train->direction = SEM_EAST;
	train->state = MOVING;
	
	sem_car car;
	sem_coordinate_set(&(car.position), 1, 0);
	car.track = &trackW_E;
	sem_train_add_car(train, &car);	

	sem_action action;
	action.time = 1000;
	action.context = train;
	action.dynamically_allocated = false;
	action.function = sem_move_train_action;
	action.function(heap, &action);

	sem_action* reverse_at_buffer_action = sem_heap_remove_earliest(heap);
	g_assert_nonnull(reverse_at_buffer_action);
	reverse_at_buffer_action->function(heap, reverse_at_buffer_action);
	g_assert_true(train->direction == SEM_WEST);
	g_assert_cmpuint(train->position->x, ==, 0);
	g_assert_cmpuint(train->position->y, ==, 0);
}

void test_input_signal_green_to_red(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);

	sem_signal* signal = malloc(sizeof(sem_signal));
	signal->type = MAIN_MANUAL;
	signal->aspect = GREEN;
	
	sem_tile_set_signal(sem_tile_at(world, 0, 0), &track, signal);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;
	input.rank = PRIMARY;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, world, &action);

	g_assert_nonnull(action);
	action->function(heap, action);

	g_assert_true(signal->aspect == RED);
}
