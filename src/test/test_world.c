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
void test_world_track_matching_inactive_points(test_world_context* test_ctx, const void* data);
void test_world_track_part_horizontal_first(test_world_context* test_ctx, const void* data);
void test_world_track_part_horizontal_second(test_world_context* test_ctx, const void* data);
void test_world_track_part_vertical(test_world_context* test_ctx, const void* data);

void add_test_world(const char *test_name, void (*test)(test_world_context*, const void* data));
void test_world_setup(test_world_context* test_ctx, const void* data);
void test_world_teardown(test_world_context* test_ctx, const void* data);

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
	add_test_world("/world/track_matching_inactive_points", test_world_track_matching_inactive_points);
	add_test_world("/world/track_part_horizontal_first", test_world_track_part_horizontal_first);
	add_test_world("/world/track_part_horizontal_second", test_world_track_part_horizontal_second);
	add_test_world("/world/track_part_vertical", test_world_track_part_vertical);
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

void test_world_track_matching_inactive_points(test_world_context* test_ctx, const void* data) {
	#pragma unused(data)
	sem_world* world = &(test_ctx->world);

	sem_tile* tile = sem_tile_at(world, 0, 0);
	sem_track active_track;
	sem_track_set(&active_track, SEM_NORTH, SEM_SOUTH);
	sem_tile_set_points(tile, &active_track);

	sem_track inactive_track;
	sem_track_set(&inactive_track, SEM_NORTH, SEM_SOUTH | SEM_WEST);
	tile->points[1] = &inactive_track;

	sem_track* matched_track = NULL;
	sem_tile_track_matching(tile, &inactive_track, &matched_track);

	g_assert_false(matched_track == NULL);
	g_assert_true(matched_track == &inactive_track);
}

void test_world_track_part_horizontal_first(test_world_context* test_ctx, const void* data) {
	#pragma unused(test_ctx)
	#pragma unused(data)
	sem_track trackSW_NE;
	sem_track_set(&trackSW_NE, SEM_SOUTH | SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	trackW_E.next = &trackSW_NE;

	g_assert_true(sem_track_part_horizontal(&trackW_E) == &trackW_E);
}

void test_world_track_part_horizontal_second(test_world_context* test_ctx, const void* data) {
	#pragma unused(test_ctx)
	#pragma unused(data)
	sem_track trackSW_NE;
	sem_track_set(&trackSW_NE, SEM_SOUTH | SEM_WEST, SEM_NORTH | SEM_EAST);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	trackSW_NE.next = &trackW_E;

	g_assert_true(sem_track_part_horizontal(&trackSW_NE) == &trackW_E);
}

void test_world_track_part_vertical(test_world_context* test_ctx, const void* data) {
	#pragma unused(test_ctx)
	#pragma unused(data)
	sem_track trackS_N;
	sem_track_set(&trackS_N, SEM_SOUTH, SEM_NORTH);

	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	trackW_E.next = &trackS_N;

	g_assert_true(sem_track_part_vertical(&trackW_E) == &trackS_N);
}
