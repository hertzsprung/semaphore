#include <glib.h>
#include <stdio.h>

#include "test_heap.h"

#include "sem_error.h"
#include "sem_heap.h"

void add_test_heap(const char *test_name, void (*test)(sem_heap* heap, const void* data));

void test_heap_setup(sem_heap* heap, const void* data);
void test_heap_teardown(sem_heap* heap, const void* data);

void test_heap_add_and_get_one_entry_at_tail(sem_heap* heap, const void* data);
void test_heap_add_remove_add_entry_to_tail(sem_heap* heap, const void* data);
void test_heap_remove_tail_error_if_empty(sem_heap* heap, const void* data);
void test_heap_add_two_entries_to_tail(sem_heap* heap, const void* data);
void test_heap_orders_entries_reverse_chronologically(sem_heap* heap, const void* data);

void test_heap_setup(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_init(heap);
}

void test_heap_teardown(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_destroy(heap);
}

void add_tests_heap() {
	add_test_heap("/heap/add_and_get_one_entry_at_tail", test_heap_add_and_get_one_entry_at_tail);
	add_test_heap("/heap/add_remove_add_entry_to_tail", test_heap_add_remove_add_entry_to_tail);
	add_test_heap("/heap/remove_tail_error_if_empty", test_heap_remove_tail_error_if_empty);
	add_test_heap("/heap/add_two_entries_to_tail", test_heap_add_two_entries_to_tail);
	add_test_heap("/heap/orders_entries_reverse_chronologically", test_heap_orders_entries_reverse_chronologically);
}

void add_test_heap(const char *test_name, void (*test)(sem_heap*, const void* data)) {
	g_test_add(test_name, sem_heap, NULL, test_heap_setup, test, test_heap_teardown);
}

void test_heap_add_and_get_one_entry_at_tail(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_entry e;

	sem_heap_add_tail(heap, &e);

	g_assert_true(heap->entries[0] == &e);
}

void test_heap_add_remove_add_entry_to_tail(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_entry e1, e2;

	sem_heap_add_tail(heap, &e1);
	sem_heap_remove_tail(heap);
	sem_heap_add_tail(heap, &e2);

	g_assert_true(heap->entries[0] == &e2);
}

void test_heap_remove_tail_error_if_empty(sem_heap* heap, const void* data) {
	#pragma unused(data)
	g_assert_true(sem_heap_remove_tail(heap) == SEM_ERROR);
}

void test_heap_add_two_entries_to_tail(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_entry e1, e2;

	sem_heap_add_tail(heap, &e1);
	sem_heap_add_tail(heap, &e2);

	g_assert_true(heap->entries[0] == &e1);
	g_assert_true(heap->entries[1] == &e2);
}

void test_heap_orders_entries_reverse_chronologically(sem_heap* heap, const void* data) {
	#pragma unused(data)

	sem_heap_entry e1, e2;
	e1.time = 3000L;
	e2.time = 2000L;

	sem_heap_insert(heap, &e1);
	sem_heap_insert(heap, &e2);

	g_assert_true(heap->entries[0] == &e2);
	g_assert_true(heap->entries[1] == &e1);
}
