#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "test_track_cache.h"
#include "sem_track_cache.h"

void test_track_cache_create_uncached_track(sem_track_cache* track_cache, const void* data);
void test_track_cache_return_cached_track(sem_track_cache* track_cache, const void* data);

void add_tests_track_cache() {
	add_test_track_cache("/track_cache/create_uncached_track", test_track_cache_create_uncached_track);
	add_test_track_cache("/track_cache/return_cached_track", test_track_cache_return_cached_track);
}

void add_test_track_cache(const char *test_name, void (*test)(sem_track_cache* track_cache, const void* data)) {
	g_test_add(test_name, sem_track_cache, NULL, test_track_cache_setup, test, test_track_cache_teardown);
}

void test_track_cache_setup(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	sem_track_cache_init(track_cache);
}

void test_track_cache_teardown(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	sem_track_cache_destroy(track_cache);
}

void test_track_cache_create_uncached_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "N-S";
	sem_track* track = NULL;
	sem_track_cache_find(track_cache, track_description, &track);

	g_assert_nonnull(track);
	g_assert_true(track->start == SEM_NORTH);
	g_assert_true(track->end == SEM_SOUTH);
}

void test_track_cache_return_cached_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "N-S";

	sem_track* track_N_S = malloc(sizeof(sem_track));
	sem_track_set(track_N_S, SEM_NORTH, SEM_SOUTH);
	g_hash_table_insert(track_cache->table, strdup(track_description), track_N_S);

	sem_track* cached_track = NULL;
	sem_track_cache_find(track_cache, track_description, &cached_track);

	g_assert_true(cached_track == track_N_S);
}
