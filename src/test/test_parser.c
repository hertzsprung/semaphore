#include <glib.h>
#include <stdlib.h>
#include <stdio.h>

#include "test_parser.h"
#include "test_track_cache.h"
#include "sem_compass.h"
#include "sem_parser.h"
#include "sem_strings.h"
#include "sem_world.h"

void test_parser_bad_tile_class(void);
void test_parser_parse_n_s_track(sem_track_cache* track_cache, const void* data);
void test_parser_e_w_track(sem_track_cache* track_cache, const void* data);
void test_parser_parse_n_s_e_w_track(void);
void test_parser_two_way_points(sem_track_cache* track_cache, const void* data);
void test_parser_red_main_auto_signal(sem_track_cache* track_cache, const void* data);
void test_parser_green_main_manual_signal(sem_track_cache* track_cache, const void* data);
void test_parser_amber_sub_signal(sem_track_cache* track_cache, const void* data);
void test_parser_print_ne_sw_track(void);
void test_parser_print_n_s_e_w_track(void);
void test_parser_print_red_main_auto_signal(void);

void add_tests_parser() {
	g_test_add_func("/parser/bad_tile_class", test_parser_bad_tile_class);
	add_test_track_cache("/parser/parse_n_s_track", test_parser_parse_n_s_track);
	add_test_track_cache("/parser/e_w_track", test_parser_e_w_track);
	g_test_add_func("/parser/parse_n_s_e_w_track", test_parser_parse_n_s_e_w_track);
	add_test_track_cache("/parser/two_way_points", test_parser_two_way_points);
	add_test_track_cache("/parser/red_main_auto_signal", test_parser_red_main_auto_signal);
	add_test_track_cache("/parser/green_main_manual_signal", test_parser_green_main_manual_signal);
	add_test_track_cache("/parser/amber_sub_signal", test_parser_amber_sub_signal);
	g_test_add_func("/parser/print_ne_sw_track", test_parser_print_ne_sw_track);
	g_test_add_func("/parser/print_n_s_e_w_track", test_parser_print_n_s_e_w_track);
	g_test_add_func("/parser/print_red_main_auto_signal", test_parser_print_red_main_auto_signal);
}

void test_parser_bad_tile_class() {
	char track_description[16] = "badclass";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	g_assert_true(sem_tile_parse(&tile, &tokens, NULL) != SEM_OK);
}

void test_parser_parse_n_s_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track N-S";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_NORTH);
	g_assert_true(tile.track->end == SEM_SOUTH);
}

void test_parser_e_w_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track E-W";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_EAST);
	g_assert_true(tile.track->end == SEM_WEST);
}

void test_parser_parse_n_s_e_w_track() {
	char track_description[16] = "track N-S+E-W";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_track* track;
	sem_track_parse(track_description, &track);

	g_assert_true(track->start == SEM_NORTH);
	g_assert_true(track->end == SEM_SOUTH);
	g_assert_true(track->next->start == SEM_EAST);
	g_assert_true(track->next->end == SEM_WEST);

	free(track);
}

void test_parser_two_way_points(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[32] = "points W-E W-NE";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == POINTS);
	g_assert_true(tile.track->start == SEM_WEST);
	g_assert_true(tile.track->end == SEM_EAST);

	g_assert_true(tile.points[0]->start == SEM_WEST);
	g_assert_true(tile.points[0]->end == SEM_EAST);
}

void test_parser_red_main_auto_signal(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[32] = "signal SW-NE red main auto";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == SIGNAL);
	g_assert_true(tile.track->start == (SEM_SOUTH | SEM_WEST));
	g_assert_true(tile.track->end == (SEM_NORTH | SEM_EAST));
	g_assert_true(tile.signal->aspect == RED);
	g_assert_true(tile.signal->type == MAIN_AUTO);
}

void test_parser_green_main_manual_signal(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[32] = "signal W-E green main manual";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == SIGNAL);
	g_assert_true(tile.track->start == SEM_WEST);
	g_assert_true(tile.track->end == SEM_EAST);
	g_assert_true(tile.signal->aspect == GREEN);
	g_assert_true(tile.signal->type == MAIN_MANUAL);
}

void test_parser_amber_sub_signal(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[32] = "signal W-E amber sub";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.signal->aspect == AMBER);
	g_assert_true(tile.signal->type == SUB);
}

void test_parser_print_ne_sw_track() {
	char expected_description[32] = "track NE-SW";
	char actual_description[32];
	FILE* out = fmemopen(actual_description, 32*sizeof(char), "w");

	sem_tile tile;
	sem_track track;
	sem_track_set(&track, SEM_NORTH | SEM_EAST, SEM_SOUTH | SEM_WEST);
	sem_tile_set_track(&tile, &track);

	sem_tile_print(out, &tile);

	fclose(out);

	g_assert_cmpstr(expected_description, ==, actual_description);
}

void test_parser_print_n_s_e_w_track() {
	char expected_description[32] = "track N-S+E-W";
	char actual_description[32];
	FILE* out = fmemopen(actual_description, 32*sizeof(char), "w");

	sem_tile tile;

	sem_track trackN_S;
	sem_track_set(&trackN_S, SEM_NORTH, SEM_SOUTH);
	sem_tile_set_track(&tile, &trackN_S);

	sem_track trackE_W;
	sem_track_set(&trackE_W, SEM_EAST, SEM_WEST);
	trackN_S.next = &trackE_W;

	sem_tile_print(out, &tile);

	fclose(out);

	g_assert_cmpstr(expected_description, ==, actual_description);
}

void test_parser_print_red_main_auto_signal() {
	char expected_description[32] = "signal N-S red main auto";
	char actual_description[32];
	FILE* out = fmemopen(actual_description, 32*sizeof(char), "w");

	sem_tile tile;
	sem_track track;
	sem_track_set(&track, SEM_NORTH, SEM_SOUTH);
	sem_signal signal;
	signal.type = MAIN_AUTO;
	signal.aspect = RED;
	sem_tile_set_signal(&tile, &track, &signal);

	sem_tile_print(out, &tile);

	fclose(out);

	g_assert_cmpstr(expected_description, ==, actual_description);
}
