#include <glib.h>

#include "test_track_cache.h"
#include "sem_track_cache.h"

void add_test_track_cache(const char *test_name, void (*test)(sem_track_cache* track_cache, const void* data));
void test_track_cache_setup(sem_track_cache* track_cache, const void* data);
void test_track_cache_teardown(sem_track_cache* track_cache, const void* data);
void test_track_cache_create_uncached_track(sem_track_cache* track_cache, const void* data);

void add_tests_track_cache() {
	add_test_track_cache("/track_cache/create_uncached_track", test_track_cache_create_uncached_track);
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
	sem_track* track = NULL;
	sem_track_cache_find(track_cache, "N-S", &track);

	g_assert_nonnull(track);
}
