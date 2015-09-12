#include <stdlib.h>
#include <glib.h>

#include "test_signal.h"

#include "sem_heap.h"
#include "sem_input.h"
#include "sem_world.h"
#include "sem_train.h"

typedef struct {
	sem_world world;
	sem_train* train;
	sem_signal* signal;
	sem_track* track;
} test_signal_context;

void test_signal_main_becomes_red_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_green_sub_becomes_amber_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_red_sub_remains_red_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_train_stops_behind_red_main(test_signal_context* test_ctx, const void* data);
void test_signal_train_medium_to_stop_behind_red_sub(test_signal_context* test_ctx, const void* data);
void test_signal_train_slow_to_stop_behind_red_sub(test_signal_context* test_ctx, const void* data);
void test_signal_train_fast_to_slow_at_red_sub(test_signal_context* test_ctx, const void* data);
void test_signal_train_slows_at_amber_main_manual(test_signal_context* test_ctx, const void* data);
void test_signal_train_slows_at_amber_main_auto(test_signal_context* test_ctx, const void* data);
void test_signal_train_medium_speed_at_amber_sub(test_signal_context* test_ctx, const void* data);
void test_signal_train_fast_at_green(test_signal_context* test_ctx, const void* data);

void test_signal_aspect_upon_accepting_train(test_signal_context* test_ctx, sem_signal_aspect before, sem_signal_type signal_type, sem_signal_aspect after);
void test_signal_train_stop(test_signal_context* test_ctx, sem_signal_type signal_type, sem_train_speed speed);
void test_signal_train_speed_change(test_signal_context* test_ctx, sem_signal_aspect aspect, sem_signal_type signal_type, sem_train_speed before, sem_train_speed after, uint64_t next_action_time);

void add_test_signal(const char *test_name, void (*test)(test_signal_context*, const void* data));
void test_signal_setup(test_signal_context* test_ctx, const void* data);
void test_signal_teardown(test_signal_context* test_ctx, const void* data);

void add_tests_signal() {
	add_test_signal("/signal/main_becomes_red_upon_accepting_train", test_signal_main_becomes_red_upon_accepting_train);
	add_test_signal("/signal/green_sub_becomes_amber_upon_accepting_train", test_signal_green_sub_becomes_amber_upon_accepting_train);
	add_test_signal("/signal_red_sub_remains_red_upon_accepting_train", test_signal_red_sub_remains_red_upon_accepting_train);
	add_test_signal("/signal/train_stops_behind_red_main", test_signal_train_stops_behind_red_main);
	add_test_signal("/signal/train_medium_to_stop_behind_red_sub", test_signal_train_medium_to_stop_behind_red_sub);
	add_test_signal("/signal/train_slow_to_stop_behind_red_sub", test_signal_train_medium_to_stop_behind_red_sub);
	add_test_signal("/signal/train_fast_to_slow_at_red_sub", test_signal_train_fast_to_slow_at_red_sub);
	add_test_signal("/signal/train_slows_at_amber_main_manual", test_signal_train_slows_at_amber_main_manual);
	add_test_signal("/signal/train_slows_at_amber_main_auto", test_signal_train_slows_at_amber_main_auto);
	add_test_signal("/signal/train_medium_speed_at_amber_sub", test_signal_train_medium_speed_at_amber_sub);
	add_test_signal("/signal/train_fast_at_green", test_signal_train_fast_at_green);
}

void add_test_signal(const char *test_name, void (*test)(test_signal_context*, const void* data)) {
	g_test_add(test_name, test_signal_context, NULL, test_signal_setup, test, test_signal_teardown);
}

void test_signal_setup(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->world.max_x = 4;
	test_ctx->world.max_y = 4;
	sem_world_init_blank(&(test_ctx->world));

	test_ctx->train = malloc(sizeof(sem_train));
	sem_train_init(test_ctx->train);

	sem_world_add_train(&(test_ctx->world), test_ctx->train);

	test_ctx->track = malloc(sizeof(sem_track));
	sem_track_set(test_ctx->track, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(&(test_ctx->world), 0, 0);
	sem_tile_set_track(tile, test_ctx->track);

	test_ctx->signal = malloc(sizeof(sem_signal));
	tile = sem_tile_at(&(test_ctx->world), 1, 0);
	sem_tile_set_signal(tile, test_ctx->track, test_ctx->signal);
}

void test_signal_teardown(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->world));
	free(test_ctx->track);
}

void test_signal_main_becomes_red_upon_accepting_train(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_signal_aspect_upon_accepting_train(test_ctx, GREEN, MAIN_MANUAL, RED);
}

void test_signal_green_sub_becomes_amber_upon_accepting_train(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_signal_aspect_upon_accepting_train(test_ctx, GREEN, SUB, AMBER);
}

void test_signal_red_sub_remains_red_upon_accepting_train(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_signal_aspect_upon_accepting_train(test_ctx, RED, SUB, RED);
}

void test_signal_aspect_upon_accepting_train(test_signal_context* test_ctx, sem_signal_aspect before, sem_signal_type signal_type, sem_signal_aspect after) {
	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);

	train->direction = SEM_EAST;

	signal->aspect = before;
	signal->type = signal_type;

	sem_tile_acceptance acceptance;
	g_assert_true(sem_tile_accept(train, sem_tile_at(world, 1, 0), &acceptance) == SEM_OK);
	g_assert_cmpuint(signal->aspect, ==, after);	
}

void test_signal_train_stops_behind_red_main(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_stop(test_ctx, MAIN_MANUAL, MEDIUM);
}

void test_signal_train_medium_to_stop_behind_red_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_stop(test_ctx, SUB, MEDIUM);
}

void test_signal_train_slow_to_stop_behind_red_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_stop(test_ctx, SUB, SLOW);
}

void test_signal_train_stop(test_signal_context* test_ctx, sem_signal_type signal_type, sem_train_speed speed) {
	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;

	train->direction = SEM_EAST;
	train->state = MOVING;
	train->speed = speed;
	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = test_ctx->track;
	sem_train_add_car(train, &car);

	signal->aspect = RED;
	signal->type = signal_type;

	sem_train_move_outcome outcome;
	g_assert_true(sem_train_move(train, &outcome) == SEM_OK);
	g_assert_true(train->state == STOPPED);
}

void test_signal_train_fast_to_slow_at_red_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_speed_change(test_ctx, RED, SUB, FAST, SLOW, 4000);
}

void test_signal_train_slows_at_amber_main_manual(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_speed_change(test_ctx, AMBER, MAIN_MANUAL, FAST, SLOW, 4000);
}

void test_signal_train_slows_at_amber_main_auto(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_speed_change(test_ctx, AMBER, MAIN_AUTO, FAST, SLOW, 4000);
}

void test_signal_train_medium_speed_at_amber_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_speed_change(test_ctx, AMBER, SUB, FAST, MEDIUM, 3500);
}

void test_signal_train_fast_at_green(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_train_speed_change(test_ctx, GREEN, SUB, MEDIUM, FAST, 3000);
}

void test_signal_train_speed_change(test_signal_context* test_ctx, sem_signal_aspect aspect, sem_signal_type signal_type, sem_train_speed before, sem_train_speed after, uint64_t next_action_time) {
	sem_world* world = &(test_ctx->world);
	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;

	train->direction = SEM_EAST;
	train->state = MOVING;
	train->speeds[0] = 1000;
	train->speeds[1] = 1500;
	train->speeds[2] = 2000;
	train->speed = before;
	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = test_ctx->track;
	sem_train_add_car(train, &car);

	signal->aspect = aspect;
	signal->type = signal_type;

	sem_action action;
	action.time = 2000;
	action.context = train;
	action.function = sem_move_train_action;
	action.dynamically_allocated = false;

	sem_move_train_action(world->actions, &action);
	sem_action* move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);

	g_assert_cmpuint(train->speed, ==, after);
	g_assert_cmpuint(move_action->time, ==, next_action_time);
}
