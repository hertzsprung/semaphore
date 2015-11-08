#include <stdlib.h>
#include <glib.h>

#include "test_signal.h"

#include "sem_heap.h"
#include "sem_input.h"
#include "sem_world.h"
#include "sem_train.h"

typedef struct {
	sem_game game;
	sem_train* train;
	sem_signal* signal;
	sem_signal* signal1;
	sem_signal* signal2;
	sem_signal* signal3;
	sem_track* track;
} test_signal_context;

void test_signal_main_becomes_red_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_green_sub_becomes_amber_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_amber_sub_becomes_amber_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_red_sub_remains_red_upon_accepting_train(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_sub_clears_previous_sub(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_sub_preserves_previous_manual_amber_sub(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_leaves_amber_sub_behind_red_main_auto(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_sub_leaves_previous_main_red(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_auto_clears_previous_main_auto(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_auto_does_not_clear_previous_main_manual(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_manual_clears_previous_main_auto(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_auto_clears_previous_main_auto_with_sub_inbetween(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_auto_clears_sub_behind_previous_main_auto(test_signal_context* test_ctx, const void* data);
void test_signal_clearing_main_auto_does_not_clear_sub_behind_previous_main_manual(test_signal_context* test_ctx, const void* data);
void test_signal_exiting_portal_clears_previous_sub(test_signal_context* test_ctx, const void* data);
void test_signal_exiting_portal_does_not_clear_previous_main_manual(test_signal_context* test_ctx, const void* data);
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
void test_signal_aspects_after_clearing_pair(test_signal_context* test_ctx,
		sem_signal_type signal1_type, sem_signal_aspect signal1_before, 
		sem_signal_type signal2_type, sem_signal_aspect signal2_before, 
		sem_signal_aspect signal1_after, sem_signal_aspect signal2_after);
void test_signal_aspects_after_clearing_triple(test_signal_context* test_ctx,
		sem_signal_type signal1_type, sem_signal_aspect signal1_before, 
		sem_signal_type signal2_type, sem_signal_aspect signal2_before, 
		sem_signal_type signal3_type, sem_signal_aspect signal3_before, 
		sem_signal_aspect signal1_after, sem_signal_aspect signal2_after, sem_signal_aspect signal3_after);
void test_signal_exiting_portal(test_signal_context* test_ctx, sem_signal_type signal_type, sem_signal_aspect after);

void add_test_signal(const char *test_name, void (*test)(test_signal_context*, const void* data));
void test_signal_setup(test_signal_context* test_ctx, const void* data);
void test_signal_teardown(test_signal_context* test_ctx, const void* data);

void add_tests_signal() {
	add_test_signal("/signal/main_becomes_red_upon_accepting_train", test_signal_main_becomes_red_upon_accepting_train);
	add_test_signal("/signal/green_sub_becomes_amber_upon_accepting_train", test_signal_green_sub_becomes_amber_upon_accepting_train);
	add_test_signal("/signal/amber_sub_becomes_amber_upon_accepting_train", test_signal_amber_sub_becomes_amber_upon_accepting_train);
	add_test_signal("/signal_red_sub_remains_red_upon_accepting_train", test_signal_red_sub_remains_red_upon_accepting_train);
	add_test_signal("/signal/clearing_sub_clears_previous_sub", test_signal_clearing_sub_clears_previous_sub);
	add_test_signal("/signal/clearing_sub_preserves_previous_manual_amber_sub", test_signal_clearing_sub_preserves_previous_manual_amber_sub);
	add_test_signal("/signal/clearing_main_leaves_amber_sub_behind_red_main_auto", test_signal_clearing_main_leaves_amber_sub_behind_red_main_auto);
	add_test_signal("/signal/clearing_sub_leaves_previous_main_red", test_signal_clearing_sub_leaves_previous_main_red);
	add_test_signal("/signal/clearing_main_auto_clears_previous_main_auto", test_signal_clearing_main_auto_clears_previous_main_auto);
	add_test_signal("/signal/clearing_main_manual_clears_previous_main_auto", test_signal_clearing_main_manual_clears_previous_main_auto);
	add_test_signal("/signal/clearing_main_auto_does_not_clear_previous_main_manual", test_signal_clearing_main_auto_does_not_clear_previous_main_manual);
	add_test_signal("/signal/clearing_main_auto_clears_previous_main_auto_with_sub_inbetween", test_signal_clearing_main_auto_clears_previous_main_auto_with_sub_inbetween);
	add_test_signal("/signal/clearing_main_auto_clears_sub_behind_previous_main_auto", test_signal_clearing_main_auto_clears_sub_behind_previous_main_auto);
	add_test_signal("/signal/clearing_main_auto_does_not_clear_sub_behind_previous_main_manual", test_signal_clearing_main_auto_does_not_clear_sub_behind_previous_main_manual);
	add_test_signal("/signal/exiting_portal_clears_previous_sub", test_signal_exiting_portal_clears_previous_sub);
	add_test_signal("/signal/exiting_portal_does_not_clear_previous_main_manual", test_signal_exiting_portal_does_not_clear_previous_main_manual);
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

	test_ctx->game.world.max_x = 16;
	test_ctx->game.world.max_y = 4;
	sem_game_init_blank(&(test_ctx->game));

	test_ctx->train = malloc(sizeof(sem_train));
	sem_train_init(test_ctx->train);
	test_ctx->train->direction = SEM_EAST;
	test_ctx->train->state = MOVING;

	sem_car* head_car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(head_car->position), 1, 0);
	head_car->track = test_ctx->track;
	sem_train_add_car(test_ctx->train, head_car);

	sem_car* tail_car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(tail_car->position), 0, 0);
	tail_car->track = test_ctx->track;
	sem_train_add_car(test_ctx->train, tail_car);

	sem_world_add_train(&(test_ctx->game.world), test_ctx->train);

	test_ctx->track = malloc(sizeof(sem_track));
	sem_track_set(test_ctx->track, SEM_WEST, SEM_EAST);

	test_ctx->signal1 = malloc(sizeof(sem_signal));
	test_ctx->signal2 = malloc(sizeof(sem_signal));
	test_ctx->signal3 = malloc(sizeof(sem_signal));
	test_ctx->signal = test_ctx->signal1;

	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 0, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 1, 0), test_ctx->track);
	sem_tile_set_signal(sem_tile_at(&(test_ctx->game.world), 2, 0), test_ctx->track, test_ctx->signal1);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 3, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 4, 0), test_ctx->track);
	sem_tile_set_signal(sem_tile_at(&(test_ctx->game.world), 5, 0), test_ctx->track, test_ctx->signal2);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 6, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 7, 0), test_ctx->track);
	sem_tile_set_signal(sem_tile_at(&(test_ctx->game.world), 8, 0), test_ctx->track, test_ctx->signal3);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 9, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 10, 0), test_ctx->track);
	sem_tile_set_exit(sem_tile_at(&(test_ctx->game.world), 11, 0), test_ctx->track);
}

void test_signal_teardown(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_game_destroy(&(test_ctx->game));
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

void test_signal_amber_sub_becomes_amber_upon_accepting_train(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_signal_aspect_upon_accepting_train(test_ctx, AMBER, SUB, AMBER);
}

void test_signal_red_sub_remains_red_upon_accepting_train(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_signal_aspect_upon_accepting_train(test_ctx, RED, SUB, RED);
}

void test_signal_clearing_sub_clears_previous_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, SUB, GREEN, SUB, GREEN, GREEN, AMBER);
}

void test_signal_clearing_sub_preserves_previous_manual_amber_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, SUB, AMBER, SUB, GREEN, AMBER, AMBER);
}

void test_signal_clearing_main_leaves_amber_sub_behind_red_main_auto(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, SUB, GREEN, MAIN_AUTO, GREEN, AMBER, RED);
}

void test_signal_clearing_main_auto_clears_previous_main_auto(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, MAIN_AUTO, GREEN, MAIN_AUTO, GREEN, GREEN, RED);
}

void test_signal_clearing_main_auto_does_not_clear_previous_main_manual(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, MAIN_MANUAL, GREEN, MAIN_AUTO, GREEN, RED, RED);
}

void test_signal_clearing_main_manual_clears_previous_main_auto(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, MAIN_AUTO, GREEN, MAIN_MANUAL, GREEN, GREEN, RED);
}

void test_signal_clearing_sub_leaves_previous_main_red(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_pair(test_ctx, MAIN_AUTO, GREEN, SUB, GREEN, RED, AMBER);
}

void test_signal_clearing_main_auto_clears_previous_main_auto_with_sub_inbetween(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_triple(test_ctx, MAIN_AUTO, GREEN, SUB, GREEN, MAIN_AUTO, GREEN, GREEN, AMBER, RED);
}

void test_signal_clearing_main_auto_clears_sub_behind_previous_main_auto(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_triple(test_ctx, SUB, GREEN, MAIN_AUTO, GREEN, MAIN_AUTO, GREEN, GREEN, GREEN, RED);
}

void test_signal_clearing_main_auto_does_not_clear_sub_behind_previous_main_manual(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_aspects_after_clearing_triple(test_ctx, SUB, GREEN, MAIN_MANUAL, GREEN, MAIN_AUTO, GREEN, AMBER, RED, RED);
}

void test_signal_aspects_after_clearing_pair(test_signal_context* test_ctx,
		sem_signal_type signal1_type, sem_signal_aspect signal1_before, 
		sem_signal_type signal2_type, sem_signal_aspect signal2_before, 
		sem_signal_aspect signal1_after, sem_signal_aspect signal2_after) {
	sem_signal* signal1 = test_ctx->signal1;
	sem_signal* signal2 = test_ctx->signal2;
	sem_train* train = test_ctx->train;

	signal1->type = signal1_type;
	sem_signal_set_aspect(signal1, signal1_before);
	signal2->type = signal2_type;
	sem_signal_set_aspect(signal2, signal2_before);

	sem_train_move_outcome outcome;
	for (uint8_t i=0; i<6; i++) g_assert_true(sem_train_move(train, &outcome) == SEM_OK);

	g_assert_cmpuint(signal1->aspect, ==, signal1_after);
	g_assert_cmpuint(signal2->aspect, ==, signal2_after);
}

void test_signal_aspects_after_clearing_triple(test_signal_context* test_ctx,
		sem_signal_type signal1_type, sem_signal_aspect signal1_before, 
		sem_signal_type signal2_type, sem_signal_aspect signal2_before, 
		sem_signal_type signal3_type, sem_signal_aspect signal3_before, 
		sem_signal_aspect signal1_after, sem_signal_aspect signal2_after, sem_signal_aspect signal3_after) {
	sem_signal* signal1 = test_ctx->signal1;
	sem_signal* signal2 = test_ctx->signal2;
	sem_signal* signal3 = test_ctx->signal3;
	sem_train* train = test_ctx->train;

	signal1->type = signal1_type;
	sem_signal_set_aspect(signal1, signal1_before);
	signal2->type = signal2_type;
	sem_signal_set_aspect(signal2, signal2_before);
	signal3->type = signal3_type;
	sem_signal_set_aspect(signal3, signal3_before);

	sem_train_move_outcome outcome;
	for (uint8_t i=0; i<9; i++) g_assert_true(sem_train_move(train, &outcome) == SEM_OK);

	g_assert_cmpuint(signal1->aspect, ==, signal1_after);
	g_assert_cmpuint(signal2->aspect, ==, signal2_after);
	g_assert_cmpuint(signal3->aspect, ==, signal3_after);
}

void test_signal_exiting_portal_clears_previous_sub(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_exiting_portal(test_ctx, SUB, GREEN);
}

void test_signal_exiting_portal_does_not_clear_previous_main_manual(test_signal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_signal_exiting_portal(test_ctx, MAIN_MANUAL, RED);
}

void test_signal_exiting_portal(test_signal_context* test_ctx, sem_signal_type signal_type, sem_signal_aspect after) {
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->game.world);
	sem_signal* signal1 = test_ctx->signal1;
	sem_signal* signal2 = test_ctx->signal2;
	sem_signal* signal3 = test_ctx->signal3;

	signal1->type = SUB;
	sem_signal_set_aspect(signal1, GREEN);
	signal2->type = SUB;
	sem_signal_set_aspect(signal2, GREEN);
	signal3->type = signal_type;
	sem_signal_set_aspect(signal3, GREEN);

	sem_action action;
	action.time = 2000;
	action.game = &(test_ctx->game);
	action.context = train;
	action.function = sem_move_train_action;
	action.dynamically_allocated = false;

	g_assert_true(sem_move_train_action(world->actions, &action) == SEM_OK);

	for (uint8_t i=0; i<12; i++) {
		sem_action* next_action = sem_heap_remove_earliest(world->actions);
		g_assert_nonnull(next_action);
		g_assert_true(next_action->function(world->actions, next_action) == SEM_OK);
	}

	g_assert_cmpuint(world->trains->tail_idx, ==, 0);
	g_assert_cmpuint(signal3->aspect, ==, after);
}

void test_signal_aspect_upon_accepting_train(test_signal_context* test_ctx, sem_signal_aspect before, sem_signal_type signal_type, sem_signal_aspect after) {
	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;
	sem_world* world = &(test_ctx->game.world);

	sem_signal_set_aspect(signal, before);
	signal->type = signal_type;

	sem_tile_acceptance acceptance;
	g_assert_true(sem_tile_accept(train, sem_tile_at(world, 2, 0), &acceptance) == SEM_OK);
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

	train->speed = speed;

	sem_signal_set_aspect(signal, RED);
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
	sem_world* world = &(test_ctx->game.world);
	sem_signal* signal = test_ctx->signal;
	sem_train* train = test_ctx->train;

	train->speeds[0] = 1000;
	train->speeds[1] = 1500;
	train->speeds[2] = 2000;
	train->speed = before;

	sem_signal_set_aspect(signal, aspect);
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
