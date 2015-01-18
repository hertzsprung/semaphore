#ifndef _SEM_TEST_HEAP_ENTRY_ARRAY_H
#define _SEM_TEST_HEAP_ENTRY_ARRAY_H

#include "sem_heap.h"

void add_tests_heap_entry_array(void);
void test_heap_entry_array_add_and_get_one_entry(sem_heap_entry_array* a, const void* data);
void test_heap_entry_array_add_remove_add_entry(sem_heap_entry_array* a, const void* data);
void test_heap_entry_array_remove_error_if_empty(sem_heap_entry_array* a, const void* data);
void test_heap_entry_array_add_two_entries(sem_heap_entry_array* a, const void* data);

#endif
