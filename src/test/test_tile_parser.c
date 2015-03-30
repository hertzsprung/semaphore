#include <glib.h>

#include "test_tile_parser.h"
#include "test_track_cache.h"
#include "sem_compass.h"
#include "sem_tile_parser.h"
#include "sem_world.h"

void test_tile_parser_n_s_track(sem_track_cache* track_cache, const void* data);

void add_tests_tile_parser() {
	add_test_track_cache("/tile_parser/n_s_track", test_tile_parser_n_s_track);
}

void test_tile_parser_n_s_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track N-S";
	sem_tile tile;
	sem_tile_parse(track_description, track_cache, &tile);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_NORTH);
	g_assert_true(tile.track->end == SEM_SOUTH);
}
