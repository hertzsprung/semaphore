#ifndef _SEM_TEST_TRACK_CACHE_H
#define _SEM_TEST_TRACK_CACHE_H

#include "sem_track_cache.h"

void add_tests_track_cache(void);
void add_test_track_cache(const char *test_name, void (*test)(sem_track_cache* track_cache, const void* data));

void test_track_cache_setup(sem_track_cache* track_cache, const void* data);
void test_track_cache_teardown(sem_track_cache* track_cache, const void* data);

#endif

