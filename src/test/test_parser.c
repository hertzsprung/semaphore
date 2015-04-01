#include <glib.h>

#include "test_parser.h"
#include "test_track_cache.h"
#include "sem_compass.h"
#include "sem_parser.h"
#include "sem_world.h"

void test_parser_n_s_track(sem_track_cache* track_cache, const void* data);
void test_parser_e_w_track(sem_track_cache* track_cache, const void* data);

void add_tests_parser() {
	add_test_track_cache("/parser/n_s_track", test_parser_n_s_track);
	add_test_track_cache("/parser/e_w_track", test_parser_e_w_track);
}

void test_parser_n_s_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track N-S";
	sem_tile tile;
	sem_tile_parse(&tile, track_description, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_NORTH);
	g_assert_true(tile.track->end == SEM_SOUTH);
}

void test_parser_e_w_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track E-W";
	sem_tile tile;
	sem_tile_parse(&tile, track_description, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_EAST);
	g_assert_true(tile.track->end == SEM_WEST);
}
