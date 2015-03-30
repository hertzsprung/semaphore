#include <glib.h>

#include "test_tile_parser.h"
#include "sem_compass.h"
#include "sem_tile_parser.h"
#include "sem_world.h"

void test_tile_parser_n_s_track(void);

void add_tests_tile_parser() {
	g_test_add_func("/tile_parser/n_s_track", test_tile_parser_n_s_track);
}

void test_tile_parser_n_s_track() {
	char track_description[16] = "track N-S";
	sem_tile tile;
	sem_tile_parse(track_description, NULL, &tile);

	g_assert_true(tile.class == TRACK);
	//g_assert_true(tile.track->start == SEM_NORTH);
	//g_assert_true(tile.track->end == SEM_SOUTH);
}
