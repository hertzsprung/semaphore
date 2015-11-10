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
	sem_game game;
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
void test_input_reverses_at_buffer_stopping_at_red_sub(test_input_context* test_ctx, const void* data);
void test_input_signal_green_to_red(test_input_context* test_ctx, const void* data);
void test_input_signal_not_green_to_green(test_input_context* test_ctx, const void* data);
void test_input_signal_to_amber(test_input_context* test_ctx, const void* data);
void test_input_move_train_when_signal_goes_green(test_input_context* test_ctx, const void* data);
void test_input_move_train_when_signal_goes_amber(test_input_context* test_ctx, const void* data);
void test_input_loses_revenue_on_emergency_stop(test_input_context* test_ctx, const void* data);
void test_input_no_emergency_stop_at_medium_speed(test_input_context* test_ctx, const void* data);
void test_input_continue_after_emergency_stop(test_input_context* test_ctx, const void* data);
void test_input_no_repeated_emergency_stop(test_input_context* test_ctx, const void* data);

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data));
void test_input_setup(test_input_context* test_ctx, const void* data);
void test_input_teardown(test_input_context* test_ctx, const void* data);
void test_input_signal_aspect(test_input_context* test_ctx, sem_signal_aspect before, sem_input_rank input_rank, sem_signal_aspect after);
void test_input_move_train_on_signal_aspect_change(test_input_context* test_ctx, sem_input_rank input_rank);
sem_action* test_input_stop_at_red_signal(test_input_context* test_ctx, sem_train_speed speed, int32_t expected_balance);
sem_signal* test_input_reverse_at_buffer_setup(test_input_context* test_ctx);

void add_tests_input(void) {
	add_test_input("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_input("/input/toggles_train_state", test_input_toggles_train_state);
	add_test_input("/input/reverses_train", test_input_reverses_train);
	add_test_input("/input/removes_derailed_train", test_input_removes_derailed_train);
	add_test_input("/input/unremoves_derailed_train_after_crash", test_input_unremoves_derailed_train_after_crash);
	add_test_input("/input/null_action_on_blank_tile", test_input_null_action_on_blank_tile);
	add_test_input("/input/switches_points", test_input_switches_points);
	add_test_input("/input/reverses_at_buffer", test_input_reverses_at_buffer);
	add_test_input("/input/reverses_at_buffer_stopping_at_red_sub", test_input_reverses_at_buffer_stopping_at_red_sub);
	add_test_input("/input/signal_green_to_red", test_input_signal_green_to_red);
	add_test_input("/input/signal_not_green_to_green", test_input_signal_not_green_to_green);
	add_test_input("/input/signal_to_amber", test_input_signal_to_amber);
	add_test_input("/input/move_train_when_signal_goes_green", test_input_move_train_when_signal_goes_green);
	add_test_input("/input/move_train_when_signal_goes_amber", test_input_move_train_when_signal_goes_amber);
	add_test_input("/input/loses_revenue_on_emergency_stop", test_input_loses_revenue_on_emergency_stop);
	add_test_input("/input/no_emergency_stop_at_medium_speed", test_input_no_emergency_stop_at_medium_speed);
	add_test_input("/input/continue_after_emergency_stop", test_input_continue_after_emergency_stop);
	add_test_input("/input/no_repeated_emergency_stop", test_input_no_repeated_emergency_stop);
}

void add_test_input(const char *test_name, void (*test)(test_input_context*, const void* data)) {
	g_test_add(test_name, test_input_context, NULL, test_input_setup, test, test_input_teardown);
}

void test_input_setup(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->game.world.max_x = 3;
	test_ctx->game.world.max_y = 1;
	sem_game_init_blank(&(test_ctx->game));

	test_ctx->train = malloc(sizeof(sem_train));
	sem_train_init(test_ctx->train);
	sem_world_add_train(&(test_ctx->game.world), test_ctx->train);

	sem_heap_init(&(test_ctx->heap));
}

void test_input_teardown(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->game.world));
	sem_dynamic_array_destroy(&(test_ctx->heap));
}

void test_input_null_action_for_unoccupied_coordinate(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_train* train = test_ctx->train;
	sem_game* game = &(test_ctx->game);

	sem_car car;
	sem_coordinate_set(&(car.position), 1, 4);
	sem_train_add_car(train, &car);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 3, 2);
	sem_input_event input;
	input.tile = &coord;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, game, &action);

	g_assert_null(action);
}

void test_input_toggles_train_state(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = test_ctx->train;
	sem_game* game = &(test_ctx->game);
	sem_world* world = &(test_ctx->game.world);
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

	sem_train_input_act_upon(&input, game, &action);
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
	sem_game* game = &(test_ctx->game);
	sem_train* train = test_ctx->train;
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
	sem_train_input_act_upon(&input, game, &action);

	g_assert_nonnull(action);
	action->function(heap, action);

	g_assert_true(train->direction == SEM_WEST);
}

void test_input_removes_derailed_train(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->game.world);
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
	// FIXME
}

void test_input_null_action_on_blank_tile(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_game* game = &(test_ctx->game);

	sem_tile* tile = sem_tile_at(&(game->world), 0, 0);
	tile->class = BLANK;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, game, &action);
	g_assert_null(action);
}

void test_input_switches_points(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_game* game = &(test_ctx->game);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track trackW_NE;
	sem_track_set(&trackW_NE, SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(&(game->world), 0, 0);
	sem_tile_set_points(tile, &trackW_NE);
	tile->points[0] = &trackW_E;
	tile->points[1] = &trackW_NE;

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, game, &action);
	action->function(heap, action);
	free(action);

	g_assert_true(tile->track == &trackW_E);
}

void test_input_reverses_at_buffer(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_train* train = test_ctx->train;

	test_input_reverse_at_buffer_setup(test_ctx);

	sem_action* reverse_at_buffer_action = sem_heap_remove_earliest(heap);
	g_assert_nonnull(reverse_at_buffer_action);
	reverse_at_buffer_action->function(heap, reverse_at_buffer_action);

	g_assert_true(train->direction == SEM_WEST);
	g_assert_cmpuint(train->position->x, ==, 0);
	g_assert_cmpuint(train->position->y, ==, 0);
}

void test_input_reverses_at_buffer_stopping_at_red_sub(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_train* train = test_ctx->train;

	sem_signal* signal = test_input_reverse_at_buffer_setup(test_ctx);

	sem_signal_force_aspect(signal, RED);

	sem_action* reverse_at_buffer_action = sem_heap_remove_earliest(heap);
	g_assert_nonnull(reverse_at_buffer_action);
	reverse_at_buffer_action->function(heap, reverse_at_buffer_action);

	g_assert_true(train->state == STOPPED);
	g_assert_cmpuint(train->position->x, ==, 1);
}

sem_signal* test_input_reverse_at_buffer_setup(test_input_context* test_ctx) {
	sem_world* world = &(test_ctx->game.world);
	sem_train* train = test_ctx->train;
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track* trackW_E = malloc(sizeof(sem_track));
	sem_track_set(trackW_E, SEM_WEST, SEM_EAST);

	sem_signal* signal = malloc(sizeof(sem_signal));
	sem_signal_init(signal, SUB, GREEN);
	
	sem_tile_set_signal(sem_tile_at(world, 0, 0), trackW_E, signal);
	sem_tile_set_track(sem_tile_at(world, 1, 0), trackW_E);
	sem_tile_set_buffer(sem_tile_at(world, 2, 0), trackW_E);

	train->direction = SEM_EAST;
	train->state = MOVING;
	train->speed = FAST;
	
	sem_car* car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(car->position), 1, 0);
	car->track = trackW_E;
	sem_train_add_car(train, car);	

	sem_action* action = malloc(sizeof(sem_action));
	action->time = 1000;
	action->context = train;
	action->dynamically_allocated = true;
	action->function = sem_move_train_action;
	action->function(heap, action);

	return signal;
}

void test_input_signal_green_to_red(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_input_signal_aspect(test_ctx, GREEN, PRIMARY, RED);
}

void test_input_signal_not_green_to_green(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_input_signal_aspect(test_ctx, AMBER, PRIMARY, GREEN);
}

void test_input_signal_to_amber(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_input_signal_aspect(test_ctx, GREEN, SECONDARY, AMBER);
}

void test_input_signal_aspect(test_input_context* test_ctx, sem_signal_aspect before, sem_input_rank input_rank, sem_signal_aspect after) {
	sem_game* game = &(test_ctx->game);
	sem_dynamic_array* heap = &(test_ctx->heap);

	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);

	sem_signal* signal = malloc(sizeof(sem_signal));
	sem_signal_init(signal, MAIN_MANUAL, before);
	
	sem_tile_set_signal(sem_tile_at(&(game->world), 0, 0), &track, signal);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 0, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;
	input.rank = input_rank;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, game, &action);

	g_assert_nonnull(action);
	action->function(heap, action);
	free(action);

	g_assert_true(signal->aspect == after);
}

void test_input_move_train_when_signal_goes_green(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_input_move_train_on_signal_aspect_change(test_ctx, PRIMARY);
}

void test_input_move_train_when_signal_goes_amber(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_input_move_train_on_signal_aspect_change(test_ctx, SECONDARY);
}

void test_input_move_train_on_signal_aspect_change(test_input_context* test_ctx, sem_input_rank input_rank) {
	sem_game* game = &(test_ctx->game);
	sem_world* world = &(test_ctx->game.world);
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_train* train = test_ctx->train;

	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);
	sem_tile_set_track(sem_tile_at(world, 0, 0), &track);

	sem_signal* signal = malloc(sizeof(sem_signal));
	sem_signal_init(signal, MAIN_MANUAL, RED);
	
	sem_tile_set_signal(sem_tile_at(world, 1, 0), &track, signal);

	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = &track;
	sem_train_add_car(train, &car);

	train->direction = SEM_EAST;
	train->state = MOVING;

	sem_train_move_outcome outcome;
	g_assert_true(sem_train_move(train, &outcome) == SEM_OK);
	g_assert_true(train->state == STOPPED);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 1, 0);
	sem_input_event input;
	input.time = 3000L;
	input.tile = &coord;
	input.rank = input_rank;

	sem_action* action = NULL;
	sem_tile_input_act_upon(&input, game, &action);

	g_assert_nonnull(action);
	action->function(heap, action);

	sem_action* change_train_state = sem_heap_remove_earliest(heap);
	g_assert_nonnull(change_train_state);
	change_train_state->function(heap, change_train_state);
	free(action);

	g_assert_true(train->state == MOVING);
}

void test_input_loses_revenue_on_emergency_stop(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	free(test_input_stop_at_red_signal(test_ctx, FAST, -200));
}

void test_input_no_emergency_stop_at_medium_speed(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	free(test_input_stop_at_red_signal(test_ctx, MEDIUM, 0));
}

void test_input_continue_after_emergency_stop(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_game* game = &(test_ctx->game);
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_train* train = test_ctx->train;

	sem_action* action = test_input_stop_at_red_signal(test_ctx, FAST, -200);

	sem_coordinate coord;
	sem_coordinate_set(&coord, 1, 0);
	sem_input_event input;
	input.tile = &coord;
	input.rank = PRIMARY;
	input.time = 5000;

	sem_action* toggle_signal_action = NULL;

	// toggling signal should not cause train to move
	// we verify this by checking that the action time is 25s after the time the train stopped
	g_assert_true(sem_tile_input_act_upon(&input, game, &toggle_signal_action) == SEM_OK);
	g_assert_nonnull(toggle_signal_action);
	g_assert_true(toggle_signal_action->function(heap, toggle_signal_action) == SEM_OK);

	sem_action* change_state_action = sem_heap_remove_earliest(heap);
	g_assert_nonnull(change_state_action);
	g_assert_cmpuint(change_state_action->time, ==, 29000);
	g_assert_true(change_state_action->function(heap, change_state_action) == SEM_OK);
	g_assert_true(train->state == MOVING);

	free(action);
}

void test_input_no_repeated_emergency_stop(test_input_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_game* game = &(test_ctx->game);

	sem_action* action = test_input_stop_at_red_signal(test_ctx, FAST, -200);

	sem_action* change_state_action = sem_heap_remove_earliest(heap);
	g_assert_true(change_state_action->function(heap, change_state_action) == SEM_OK);

	sem_action* move_action = sem_heap_remove_earliest(heap);
	g_assert_true(move_action->function(heap, move_action) == SEM_OK);

	g_assert_cmpint(game->revenue.balance, ==, -200);

	free(action);
}

sem_action* test_input_stop_at_red_signal(test_input_context* test_ctx, sem_train_speed speed, int32_t expected_balance) {
	sem_game* game = &(test_ctx->game);
	sem_world* world = &(test_ctx->game.world);
	sem_dynamic_array* heap = &(test_ctx->heap);
	sem_train* train = test_ctx->train;

	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);
	sem_tile_set_track(sem_tile_at(world, 0, 0), &track);

	sem_signal* signal = malloc(sizeof(sem_signal));
	sem_signal_init(signal, MAIN_MANUAL, RED);
	
	sem_tile_set_signal(sem_tile_at(world, 1, 0), &track, signal);

	sem_car* car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(car->position), 0, 0);
	car->track = &track;
	sem_train_add_car(train, car);

	train->direction = SEM_EAST;
	train->state = MOVING;
	train->speed = speed;

	sem_action* action = malloc(sizeof(sem_action));
	action->time = 4000;
	action->context = train;
	action->game = game;
	g_assert_true(sem_move_train_action(heap, action) == SEM_OK);

	g_assert_true(train->state == STOPPED);
	g_assert_cmpint(game->revenue.balance, ==, expected_balance);

	return action;
}
