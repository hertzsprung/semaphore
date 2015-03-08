#include <glib.h>

#include "test_world.h"

#include "sem_world.h"

typedef struct {
	sem_world world;
	sem_train train;
} test_world_context;

void test_world_create_blank(test_world_context* test_ctx, const void* data);
void test_world_switch_points_to_next_in_array(test_world_context* test_ctx, const void* data);
void test_world_needs_points_switch_when_train_on_inactive_track(test_world_context* test_ctx, const void* data);
void test_world_not_needs_points_switch_when_train_on_active_track(test_world_context* test_ctx, const void* data);
void test_world_train_not_accepted_on_unconnected_points(test_world_context* test_ctx, const void* data);

void test_world_setup(test_world_context* test_ctx, const void* data);
void test_world_teardown(test_world_context* test_ctx, const void* data);
void add_test_world(const char *test_name, void (*test)(test_world_context*, const void* data));

void test_world_setup(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	world->max_x = 2;
	world->max_y = 1;

	sem_world_init_blank(world);
}

void test_world_teardown(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)

	sem_world* world = &(test_ctx->world);
	sem_world_destroy(world);
}

void add_tests_world() {
	add_test_world("/world/create_blank", test_world_create_blank);
	add_test_world("/world/switch_points_to_next_in_array", test_world_switch_points_to_next_in_array);
	add_test_world("/world/needs_points_switch_when_train_on_inactive_track", test_world_needs_points_switch_when_train_on_inactive_track);
	add_test_world("/world/not_needs_points_switch_when_train_on_active_track", test_world_not_needs_points_switch_when_train_on_active_track);
	add_test_world("/world/train_not_accepted_on_unconnected_points", test_world_train_not_accepted_on_unconnected_points);
}

void add_test_world(const char *test_name, void (*test)(test_world_context*, const void* data)) {
	g_test_add(test_name, test_world_context, NULL, test_world_setup, test, test_world_teardown);
}

void test_world_create_blank(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	g_assert_true(sem_tile_at(world, 0, 0)->class == BLANK);
	g_assert_true(sem_tile_at(world, 1, 0)->class == BLANK);
}

void test_world_switch_points_to_next_in_array(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	sem_track trackW_NE;
	sem_track trackW_SE;
	sem_track trackW_E;

	sem_tile* tile = sem_tile_at(world, 0, 0);
	sem_tile_set_points(tile, &trackW_NE);
	tile->points[0] = &trackW_SE;
	tile->points[1] = &trackW_E;
	tile->points[2] = &trackW_NE;

	sem_tile_switch_points(tile);

	g_assert_true(tile->track == &trackW_SE);
}

void test_world_needs_points_switch_when_train_on_inactive_track(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);

	sem_tile_acceptance acceptance;
	sem_tile_acceptance_init(&acceptance);
	
	train->direction = SEM_EAST;

	sem_track trackNW_E;
	sem_track_set(&trackNW_E, SEM_NORTH | SEM_WEST, SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	sem_tile_set_points(tile, &trackNW_E);
	tile->points[1] = &trackW_E;

	sem_tile_accept(train, tile, &acceptance);
	g_assert_true(acceptance.need_points_switch);
}

void test_world_not_needs_points_switch_when_train_on_active_track(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);

	sem_tile_acceptance acceptance;
	sem_tile_acceptance_init(&acceptance);
	
	train->direction = SEM_EAST;

	sem_track trackNW_E;
	sem_track_set(&trackNW_E, SEM_NORTH | SEM_WEST, SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	sem_tile_set_points(tile, &trackW_E);
	tile->points[0] = &trackNW_E;
	tile->points[1] = &trackW_E;

	sem_tile_accept(train, tile, &acceptance);

	g_assert_false(acceptance.need_points_switch);
}

void test_world_train_not_accepted_on_unconnected_points(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_train* train = &(test_ctx->train);
	sem_world* world = &(test_ctx->world);

	train->direction = SEM_EAST;	

	sem_tile_acceptance acceptance;
	sem_tile_acceptance_init(&acceptance);

	sem_track trackN_SE;
	sem_track_set(&trackN_SE, SEM_NORTH, SEM_SOUTH | SEM_EAST);

	sem_track trackN_S;
	sem_track_set(&trackN_S, SEM_NORTH, SEM_SOUTH);

	sem_tile* tile = sem_tile_at(world, 1, 0);
	sem_tile_set_points(tile, &trackN_S);
	tile->points[0] = &trackN_SE;
	tile->points[1] = &trackN_S;

	g_assert_true(sem_tile_accept(train, tile, &acceptance) == SEM_ERROR);
}
