#ifndef _SEM_TEST_HEAP_H
#define _SEM_TEST_HEAP_H

#include "sem_dynamic_array.h"

void add_tests_heap(void);
void add_test_heap(const char *test_name, void (*test)(sem_dynamic_array* heap, const void* data));

#endif
