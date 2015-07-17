#include <glib.h>
#include <stdlib.h>

#include "test_portal.h"

#include "sem_heap.h"
#include "sem_portal.h"
#include "sem_world.h"

typedef struct {
	sem_world world;
	sem_track* track;
} test_portal_context;

void test_portal_single_car_train_enters_from_portal(test_portal_context* test_ctx, const void* data);

void add_test_portal(const char *test_name, void (*test)(test_portal_context*, const void* data));
void test_portal_setup(test_portal_context* test_ctx, const void* data);
void test_portal_teardown(test_portal_context* test_ctx, const void* data);

void add_tests_portal() {
	add_test_portal("/portal/single_car_train_enters_from_portal", test_portal_single_car_train_enters_from_portal);
}

void add_test_portal(const char *test_name, void (*test)(test_portal_context*, const void* data)) {
	g_test_add(test_name, test_portal_context, NULL, test_portal_setup, test, test_portal_teardown);
}

void test_portal_setup(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)

	test_ctx->world.max_x = 4;
	test_ctx->world.max_y = 4;
	sem_world_init_blank(&(test_ctx->world));

	test_ctx->track = malloc(sizeof(sem_track));
	sem_track_set(test_ctx->track, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(&(test_ctx->world), 0, 0);
	sem_tile_set_entry(tile, test_ctx->track);

	sem_tile_set_track(sem_tile_at(&(test_ctx->world), 1, 0), test_ctx->track);
}

void test_portal_teardown(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world_destroy(&(test_ctx->world));
	free(test_ctx->track);
}

void test_portal_single_car_train_enters_from_portal(test_portal_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	sem_train_entry_context context;
	context.world = world;
	sem_coordinate_set(&(context.position), 0, 0);
	context.direction = SEM_EAST;

	sem_action action;
	action.time = 2000;
	action.context = &context;
	action.dynamically_allocated = false;

	sem_train_entry_action(world->actions, &action);

	g_assert_cmpuint(world->trains->tail_idx, ==, 1);
	sem_train* train = (sem_train*) world->trains->items[0];
	g_assert_true(train->state == MOVING);

	sem_action* move_action = sem_heap_remove_earliest(world->actions);
	g_assert_nonnull(move_action);
	g_assert_true(move_action->function(world->actions, move_action) == SEM_OK);
	g_assert_cmpuint(train->position->x, ==, 1);
	g_assert_cmpuint(train->position->y, ==, 0);
}