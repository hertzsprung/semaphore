#include "test_heap_entry_array.h"

#include <glib.h>
#include <stdio.h>

#include "sem_error.h"
#include "sem_heap.h"

void add_test_heap_entry_array(const char *test_name, void (*test)(sem_heap_entry_array*, const void* data));
void test_heap_entry_array_setup(sem_heap_entry_array* a, const void* data);
void test_heap_entry_array_teardown(sem_heap_entry_array* a, const void* data);

void test_heap_entry_array_setup(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	sem_heap_entry_array_init(a);
}

void test_heap_entry_array_teardown(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	sem_heap_entry_array_destroy(a);
}

void add_tests_heap_entry_array() {
	add_test_heap_entry_array("/heap_entry_array/add_and_get_one_entry", test_heap_entry_array_add_and_get_one_entry);
	add_test_heap_entry_array("/heap_entry_array/add_remove_add_entry", test_heap_entry_array_add_remove_add_entry);
	add_test_heap_entry_array("/heap_entry_array/remove_error_if_empty", test_heap_entry_array_remove_error_if_empty);
	add_test_heap_entry_array("/heap_entry_array/add_two_entries", test_heap_entry_array_add_two_entries);
}

void add_test_heap_entry_array(const char *test_name, void (*test)(sem_heap_entry_array*, const void* data)) {
	g_test_add(test_name, sem_heap_entry_array, NULL, test_heap_entry_array_setup, test, test_heap_entry_array_teardown);
}

void test_heap_entry_array_add_and_get_one_entry(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	sem_heap_entry e;

	sem_heap_entry_array_add(a, &e);

	g_assert_true(a->entries[0] == &e);
}

void test_heap_entry_array_add_remove_add_entry(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	sem_heap_entry e1, e2;

	sem_heap_entry_array_add(a, &e1);
	sem_heap_entry_array_remove_last(a);
	sem_heap_entry_array_add(a, &e2);

	g_assert_true(a->entries[0] == &e2);
}

void test_heap_entry_array_remove_error_if_empty(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	g_assert_true(sem_heap_entry_array_remove_last(a) == SEM_ERROR);
}

void test_heap_entry_array_add_two_entries(sem_heap_entry_array* a, const void* data) {
	#pragma unused(data)
	sem_heap_entry e1, e2;

	sem_heap_entry_array_add(a, &e1);
	sem_heap_entry_array_add(a, &e2);

	g_assert_true(a->entries[0] == &e1);
	g_assert_true(a->entries[1] == &e2);
}
