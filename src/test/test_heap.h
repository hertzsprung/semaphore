#ifndef _SEM_TEST_HEAP_H
#define _SEM_TEST_HEAP_H

#include "sem_heap.h"

void add_tests_heap(void);

void add_test_heap(const char *test_name, void (*test)(sem_heap* heap, const void* data));

void test_heap_setup(sem_heap* heap, const void* data);

void test_heap_teardown(sem_heap* heap, const void* data);

#endif
