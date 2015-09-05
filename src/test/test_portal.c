#include <glib.h>
#include <stdlib.h>

#include "test_portal.h"

#include "sem_game.h"
#include "sem_heap.h"
#include "sem_input.h"
#include "sem_portal.h"
#include "sem_world.h"

typedef struct {
	sem_game game;
	sem_track* track;
} test_portal_context;

void test_portal_train_enters_from_portal(test_portal_context* test_ctx, const void* data);
void test_portal_train_exits_into_portal(test_portal_context* test_ctx, const void* data);
void test_portal_earn_revenue_when_train_exits_into_correct_portal(test_portal_context* test_ctx, const void* data);
void test_portal_earn_revenue_when_train_exits_into_wrong_portal(test_portal_context* test_ctx, const void* data);
void test_portal_earn_revenue_when_train_exits_into_portal_unexpectedly(test_portal_context* test_ctx, const void* data);
void test_portal_revenue_when_train_exits_portal(test_portal_context* test_ctx, sem_coordinate* exit, int32_t expected_balance);

void add_test_portal(const char *test_name, void (*test)(test_portal_context*, const void* data));
void test_portal_setup(test_portal_context* test_ctx, const void* data);
void test_portal_teardown(test_portal_context* test_ctx, const void* data);

void add_tests_portal() {
	add_test_portal("/portal/train_enters_from_portal", test_portal_train_enters_from_portal);
	add_test_portal("/portal/train_exits_into_portal", test_portal_train_exits_into_portal);
	add_test_portal("/portal/earn_revenue_when_train_exits_into_correct_portal", test_portal_earn_revenue_when_train_exits_into_correct_portal);
	add_test_portal("/portal/earn_revenue_when_train_exits_into_wrong_portal", test_portal_earn_revenue_when_train_exits_into_wrong_portal);
	add_test_portal("/portal/earn_revenue_when_train_exits_into_portal_unexpectedly", test_portal_earn_revenue_when_train_exits_into_portal_unexpectedly);
}

void add_test_portal(const char *test_name, void (*test)(test_portal_context*, const void* data)) {
	g_test_add(test_name, test_portal_context, NULL, test_portal_setup, test, test_portal_teardown);
}

void test_portal_setup(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->game.world.max_x = 4;
	test_ctx->game.world.max_y = 4;
	sem_game_init_blank(&(test_ctx->game));

	test_ctx->track = malloc(sizeof(sem_track));
	sem_track_set(test_ctx->track, SEM_WEST, SEM_EAST);

	sem_tile_set_entry(sem_tile_at(&(test_ctx->game.world), 0, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 1, 0), test_ctx->track);
	sem_tile_set_track(sem_tile_at(&(test_ctx->game.world), 2, 0), test_ctx->track);
	sem_tile_set_exit(sem_tile_at(&(test_ctx->game.world), 3, 0), test_ctx->track);
	sem_tile_set_exit(sem_tile_at(&(test_ctx->game.world), 3, 2), test_ctx->track);
}

void test_portal_teardown(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->game.world));
	free(test_ctx->track);
}

void test_portal_train_enters_from_portal(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->game.world);

	sem_train_entry_context context;
	sem_coordinate_set(&(context.position), 0, 0);
	context.direction = SEM_EAST;
	context.cars = 2;

	sem_action action;
	action.time = 2000;
	action.game = &(test_ctx->game);
	action.context = &context;
	action.dynamically_allocated = false;

	sem_train_entry_action(world->actions, &action);

	g_assert_cmpuint(world->trains->tail_idx, ==, 1);
	sem_train* train = (sem_train*) world->trains->items[0];
	g_assert_true(train->state == MOVING);

	sem_action* move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);
	g_assert_cmpuint(move_action->time, ==, 3000);
	g_assert_true(move_action->function(world->actions, move_action) == SEM_OK);
	g_assert_cmpuint(train->position->x, ==, 1);
	g_assert_cmpuint(train->position->y, ==, 0);
	
	g_assert_cmpuint(train->cars, ==, 2);

	move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);
	g_assert_true(move_action->function(world->actions, move_action) == SEM_OK);
	g_assert_cmpuint(train->cars, ==, 2);
}

void test_portal_train_exits_into_portal(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->game.world);

	sem_train* train = malloc(sizeof(sem_train));	
	sem_train_init(train);
	train->state = MOVING;
	train->direction = SEM_EAST;
	sem_world_add_train(world, train);

	sem_car* tail_car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(tail_car->position), 1, 0);
	tail_car->track = sem_tile_at(world, 1, 0)->track;

	sem_car* head_car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(head_car->position), 2, 0);
	head_car->track = sem_tile_at(world, 2, 0)->track;

	sem_train_add_car(train, head_car);
	sem_train_add_car(train, tail_car);

	sem_action action;
	action.time = 2000;
	action.game = &(test_ctx->game);
	action.context = train;
	action.function = sem_move_train_action;
	action.dynamically_allocated = false;

	g_assert_true(sem_move_train_action(world->actions, &action) == SEM_OK);

	sem_action* move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);
	g_assert_true(move_action->function(world->actions, move_action) == SEM_OK);

	g_assert_cmpuint(train->cars, ==, 1);
	g_assert_cmpuint(train->position->x, ==, 3);
	g_assert_cmpuint(train->position->y, ==, 0);

	move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);
	g_assert_true(move_action->function(world->actions, move_action) == SEM_OK);

	sem_action* train_exit_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(train_exit_action);
	train_exit_action->function(world->actions, move_action);

	g_assert_cmpuint(world->trains->tail_idx, ==, 0);
}

void test_portal_earn_revenue_when_train_exits_into_correct_portal(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_coordinate exit;
	sem_coordinate_set(&exit, 3, 0);
	test_portal_revenue_when_train_exits_portal(test_ctx, &exit, 200);
}

void test_portal_earn_revenue_when_train_exits_into_wrong_portal(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_coordinate exit;
	sem_coordinate_set(&exit, 3, 2);
	test_portal_revenue_when_train_exits_portal(test_ctx, &exit, -300);
}

void test_portal_earn_revenue_when_train_exits_into_portal_unexpectedly(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	test_portal_revenue_when_train_exits_portal(test_ctx, NULL, -300);
}

void test_portal_revenue_when_train_exits_portal(test_portal_context* test_ctx, sem_coordinate* exit, int32_t expected_balance) {
	sem_game* game = &(test_ctx->game);
	sem_world* world = &(test_ctx->game.world);

	sem_train* train = malloc(sizeof(sem_train));	
	sem_train_init(train);
	train->state = MOVING;
	train->direction = SEM_EAST;
	train->has_exit_position = (exit != NULL);
	if (train->has_exit_position) train->exit_position = *exit;
	sem_world_add_train(world, train);

	sem_car* head_car = malloc(sizeof(sem_car));
	sem_coordinate_set(&(head_car->position), 2, 0);
	head_car->track = sem_tile_at(world, 2, 0)->track;

	sem_train_add_car(train, head_car);

	sem_action action;
	action.time = 2000;
	action.game = &(test_ctx->game);
	action.context = train;
	action.function = sem_move_train_action;
	action.dynamically_allocated = false;

	sem_move_train_action(world->actions, &action);
	sem_action* move_action = sem_heap_remove_earliest(world->actions);
	move_action->function(world->actions, move_action);

	sem_action* train_exit_action = sem_heap_remove_earliest(world->actions);
	train_exit_action->function(world->actions, move_action);

	g_assert_cmpint(game->revenue.balance, ==, expected_balance);
}
