#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "test_serialize.h"
#include "sem_dynamic_array.h"
#include "sem_game.h"
#include "sem_heap.h"
#include "sem_input.h"
#include "sem_serialize.h"
#include "sem_serialize_actions.h"
#include "sem_world.h"

FILE* save_and_load(char* filename, sem_game* game);
void test_serialize_save_load_blank_tiles(void);
void test_serialize_save_load_track_tile(void);
void test_serialize_save_load_train(void);
void test_serialize_save_load_timer(void);
void test_serialize_load_remove_train_action(void);
void test_serialize_save_load_revenue(void);

void add_tests_serialize() {
	g_test_add_func("/serialize/save_load_blank_tiles", test_serialize_save_load_blank_tiles);
	g_test_add_func("/serialize/save_load_track_tile", test_serialize_save_load_track_tile);
	g_test_add_func("/serialize/save_load_train", test_serialize_save_load_train);
	g_test_add_func("/serialize/save_load_timer", test_serialize_save_load_timer);
	g_test_add_func("/serialize/load_remove_train_action", test_serialize_load_remove_train_action);
	g_test_add_func("/serialize/save_load_revenue", test_serialize_save_load_revenue);
}

FILE* save_and_load(char* filename, sem_game* game) {
	FILE* file = fopen(filename, "w");
	sem_serialize_save(file, game);
	sem_world_destroy(&(game->world));
	fclose(file);

	file = fopen(filename, "r");
	sem_serialize_load(file, game);
	return file;
}

void test_serialize_save_load_blank_tiles() {
	sem_game game;
	game.world.max_x = 2;
	game.world.max_y = 1;
	sem_game_init_blank(&game);

	FILE* file = save_and_load("build/test/blank_tiles", &game);
	
	g_assert_cmpuint(game.world.max_x, ==, 2);
	g_assert_cmpuint(game.world.max_y, ==, 1);

	sem_world_destroy(&(game.world));
	fclose(file);
}

void test_serialize_save_load_track_tile() {
	sem_game game;
	game.world.max_x = 1;
	game.world.max_y = 1;
	sem_game_init_blank(&game);

	sem_tile* tile = sem_tile_at(&(game.world), 0, 0);
	sem_track track;
	sem_track_set(&track, SEM_WEST, SEM_EAST);
	sem_tile_set_track(tile, &track);

	FILE* file = save_and_load("build/test/track_tile", &game);

	tile = sem_tile_at(&(game.world), 0, 0);
	g_assert_true(tile->class == TRACK);
	g_assert_true(tile->track->start == SEM_WEST);
	g_assert_true(tile->track->end == SEM_EAST);

	sem_world_destroy(&(game.world));
	fclose(file);
}

void test_serialize_save_load_train() {
	sem_game game;
	game.world.max_x = 3;
	game.world.max_y = 2;
	sem_world_init_blank(&(game.world));

	sem_tile* tile = sem_tile_at(&(game.world), 0, 1);
	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	sem_tile_set_track(tile, &trackW_E);

	tile = sem_tile_at(&(game.world), 1, 1);
	sem_track trackN_S_W_E;
	sem_track_set(&trackN_S_W_E, SEM_NORTH, SEM_SOUTH);
	trackN_S_W_E.next = &trackW_E;
	sem_tile_set_track(tile, &trackN_S_W_E);

	sem_train* saved_train1 = malloc(sizeof(sem_train));
	sem_train* train1 = saved_train1;
	sem_train_init(train1);
	train1->direction = SEM_EAST;
	train1->name = strdup("IC-123");

	sem_car car1;
	sem_coordinate_set(&(car1.position), 1, 1);
	car1.track = &trackW_E;
	sem_train_add_car(train1, &car1);

	sem_car car2;
	sem_coordinate_set(&(car2.position), 0, 1);
	car2.track = &trackW_E;
	sem_train_add_car(train1, &car2);

	sem_world_add_train(&(game.world), train1);

	sem_train* saved_train2 = malloc(sizeof(sem_train));
	sem_train* train2 = saved_train2;
	sem_train_init(train2);
	train2->direction = SEM_WEST;
	train2->state = DERAILED;
	sem_world_add_train(&(game.world), train2);

	FILE* file = save_and_load("build/test/train", &game);

	g_assert_cmpuint(game.world.trains->tail_idx, ==, 2);

	train1 = game.world.trains->items[0];
	g_assert_true(train1->state == STOPPED);
	g_assert_true(train1->direction == SEM_EAST);
	g_assert_cmpuint(train1->cars, ==, 2);
	g_assert_cmpstr(train1->name, ==, "IC-123");

	sem_car* car = train1->head_car;
	g_assert_cmpuint(car->position.x, ==, 1);
	g_assert_cmpuint(car->position.y, ==, 1);

	sem_track* track = car->track;
	g_assert_true(track->start == SEM_WEST);
	g_assert_true(track->end == SEM_EAST);

	train2 = game.world.trains->items[1];
	g_assert_true(train2->state == DERAILED);
	g_assert_true(train2->direction == SEM_WEST);

	sem_world_destroy(&(game.world));
	fclose(file);
}

void test_serialize_save_load_timer() {
	sem_game game;
	game.world.max_x = 1;
	game.world.max_y = 1;
	sem_game_init_blank(&game);
	game.world.timer->now = 123;
	game.world.timer->multiplier = 1.6;

	FILE* file = save_and_load("build/test/timer", &game);

	g_assert_cmpuint(game.world.timer->now, ==, 123);
	g_assert_cmpfloat(game.world.timer->multiplier, >, 1.59);
	g_assert_cmpfloat(game.world.timer->multiplier, <, 1.61);

	sem_world_destroy(&(game.world));
	fclose(file);
}

void test_serialize_load_remove_train_action() {
	sem_game game;
	game.world.max_x = 3;
	game.world.max_y = 1;
	sem_world_init_blank(&(game.world));

	sem_tile* tile = sem_tile_at(&(game.world), 0, 0);
	sem_track trackW_E;
	sem_track_set(&trackW_E, SEM_WEST, SEM_EAST);
	sem_tile_set_track(tile, &trackW_E);

	tile = sem_tile_at(&(game.world), 2, 0);
	sem_track trackN_S;
	sem_track_set(&trackN_S, SEM_NORTH, SEM_SOUTH);
	sem_tile_set_track(tile, &trackN_S);

	sem_train* train1 = malloc(sizeof(sem_train));
	sem_train_init(train1);
	uuid_t train1_id;
	uuid_copy(train1->id, train1_id);
	train1->direction = SEM_NORTH;
	sem_car car1;
	sem_coordinate_set(&(car1.position), 2, 0);
	car1.track = &trackN_S;
	sem_train_add_car(train1, &car1);
	sem_world_add_train(&(game.world), train1);

	sem_train* train2 = malloc(sizeof(sem_train));
	sem_train_init(train2);
	train2->state = DERAILED;
	train2->direction = SEM_EAST;
	sem_car car2;
	sem_coordinate_set(&(car2.position), 0, 0);
	car2.track = &trackW_E;
	sem_train_add_car(train2, &car2);
	sem_world_add_train(&(game.world), train2);
	
	// TODO: should fetch a partially-initialised sem_action from
	// a factory method
	sem_action action;
	action.time = 5000;
	action.function = remove_train_action;
	action.write = sem_remove_train_action_write;
	action.context = train2;

	sem_heap_insert(game.world.actions, &action);

	FILE* file = save_and_load("build/test/remove_train_action", &game);

	sem_action* loaded_action = sem_heap_remove_earliest(game.world.actions);
	g_assert_false(loaded_action == NULL);
	g_assert_true(loaded_action->function(game.world.actions, loaded_action) == SEM_OK);

	g_assert_cmpuint(game.world.trains->tail_idx, ==, 1);

	sem_train* remaining_train = (sem_train*) game.world.trains->items[0];
	g_assert_true(uuid_compare(remaining_train->id, train1_id) == 0);

	sem_world_destroy(&(game.world));
	fclose(file);
}

void test_serialize_save_load_revenue() {
	sem_game game;
	game.world.max_x = 1;
	game.world.max_y = 1;
	sem_game_init_blank(&game);
	game.revenue.balance = 2948;

	FILE* file = fopen("build/test/revenue", "w");
	sem_serialize_save(file, &game);
	sem_world_destroy(&(game.world));
	fclose(file);

	game.revenue.balance = 4911;

	file = fopen("build/test/revenue", "r");
	sem_serialize_load(file, &game);

	g_assert_cmpuint(game.revenue.balance, ==, 2948);
	fclose(file);
}

