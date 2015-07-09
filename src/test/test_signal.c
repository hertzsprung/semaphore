#include <stdlib.h>
#include <glib.h>

#include "test_signal.h"

#include "sem_world.h"
#include "sem_train.h"

typedef struct {
	sem_world world;
	sem_train* train;
	sem_signal* signal;
	sem_track* track;
} test_signal_context;

void test_signal_main_becomes_red_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_train_stops_behind_red_main(test_signal_context* test_ctx, const void* data);

void add_test_signal(const char *test_name, void (*test)(test_signal_context*, const void* data));
void test_signal_setup(test_signal_context* test_ctx, const void* data);
void test_signal_teardown(test_signal_context* test_ctx, const void* data);

void add_tests_signal() {
	add_test_signal("/signal/main_becomes_red_upon_accepting_train", test_signal_main_becomes_red_upon_accepting_train);
	add_test_signal("/signal/train_stops_behind_red_main", test_signal_train_stops_behind_red_main);
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

	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->world);

	train->direction = SEM_EAST;

	signal->aspect = GREEN;
	signal->type = MAIN_MANUAL;

	sem_tile_acceptance acceptance;
	g_assert_true(sem_tile_accept(train, sem_tile_at(world, 1, 0), &acceptance) == SEM_OK);
	g_assert_true(signal->aspect == RED);	
}

void test_signal_train_stops_behind_red_main(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;

	train->direction = SEM_EAST;
	train->state = MOVING;
	sem_car car;
	sem_coordinate_set(&(car.position), 0, 0);
	car.track = test_ctx->track;
	sem_train_add_car(train, &car);

	signal->aspect = RED;
	signal->type = MAIN_MANUAL;

	sem_train_move_outcome outcome;
	g_assert_true(sem_train_move(train, &outcome) == SEM_OK);
	g_assert_true(train->state == STOPPED);
}
