#include <glib.h>

#include "test_heap.h"

#include "sem_error.h"
#include "sem_heap.h"

void test_heap_removes_earliest(sem_heap* heap, const void* data);
void test_heap_removes_nothing_in_empty_heap(sem_heap* heap, const void* data);

void test_heap_setup(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_init(heap);
}

void test_heap_teardown(sem_heap* heap, const void* data) {
	#pragma unused(data)
	sem_heap_destroy(heap);
}

void add_tests_heap() {
	add_test_heap("/heap/removes_earliest", test_heap_removes_earliest);
	add_test_heap("/heap/removes_nothing_in_empty_heap", test_heap_removes_nothing_in_empty_heap);
}

void add_test_heap(const char *test_name, void (*test)(sem_heap*, const void* data)) {
	g_test_add(test_name, sem_heap, NULL, test_heap_setup, test, test_heap_teardown);
}

void test_heap_removes_earliest(sem_heap* heap, const void* data) {
	#pragma unused(data)

	sem_heap_entry e1, e2, e3;
	e1.time = 2000L;
	e2.time = 3000L;
	e3.time = 1000L;

	sem_heap_insert(heap, &e1);
	sem_heap_insert(heap, &e2);
	sem_heap_insert(heap, &e3);
	
	g_assert_true(sem_heap_remove_earliest(heap) == &e3);
	g_assert_true(sem_heap_remove_earliest(heap) == &e1);
	g_assert_true(sem_heap_remove_earliest(heap) == &e2);
}

void test_heap_removes_nothing_in_empty_heap(sem_heap* heap, const void* data) {
	#pragma unused(data)

	g_assert_null(sem_heap_remove_earliest(heap));
}
