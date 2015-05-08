#include <glib.h>

#include "test_dynamic_array.h"
#include "sem_dynamic_array.h"

void add_test_dynamic_array(const char *test_name, void (*test)(sem_dynamic_array* a, const void* data));
void test_dynamic_array_setup(sem_dynamic_array* a, const void* data);
void test_dynamic_array_teardown(sem_dynamic_array* a, const void* data);

void test_dynamic_array_add_item(sem_dynamic_array* a, const void* data);
void test_dynamic_array_add_two_items(sem_dynamic_array* a, const void* data);
void test_dynamic_array_remove_item(sem_dynamic_array* a, const void* data);
void test_dynamic_array_remove_nonexistent_item(sem_dynamic_array* a, const void* data);

void add_tests_dynamic_array() {
	add_test_dynamic_array("/dynamic_array/add_item", test_dynamic_array_add_item);
	add_test_dynamic_array("/dynamic_array/add_item_two_items", test_dynamic_array_add_two_items);
	add_test_dynamic_array("/dynamic_array/remove_item", test_dynamic_array_remove_item);
	add_test_dynamic_array("/dynamic_array/remove_nonexistent_item", test_dynamic_array_remove_nonexistent_item);
}

void add_test_dynamic_array(const char *test_name, void (*test)(sem_dynamic_array* a, const void* data)) {
	g_test_add(test_name, sem_dynamic_array, NULL, test_dynamic_array_setup, test, test_dynamic_array_teardown);
}

void test_dynamic_array_setup(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)
	sem_dynamic_array_init(a);
}

void test_dynamic_array_teardown(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)
	sem_dynamic_array_destroy(a);
}

void test_dynamic_array_add_item(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)

	char* item = "hello";
	sem_dynamic_array_add(a, item);
	g_assert_true(a->items[0] == item);
}

void test_dynamic_array_add_two_items(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)

	char* item1 = "hello";
	char* item2 = "world";

	sem_dynamic_array_add(a, item1);
	sem_dynamic_array_add(a, item2);

	g_assert_true(a->items[1] == item2);
}

void test_dynamic_array_remove_item(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)

	char* item1 = "hello";
	char* item2 = "world";

	sem_dynamic_array_add(a, item1);
	sem_dynamic_array_add(a, item2);
	sem_dynamic_array_remove(a, item1);

	g_assert_cmpuint(a->tail_idx, ==, 1);
	g_assert_true(a->items[0] == item2);
}

void test_dynamic_array_remove_nonexistent_item(sem_dynamic_array* a, const void* data) {
	#pragma unused(data)

	char* item1 = "hello";
	char* item2 = "world";
	char* other = "foo";

	sem_dynamic_array_add(a, item1);
	sem_dynamic_array_add(a, item2);
	g_assert_true(sem_dynamic_array_remove(a, other) == SEM_ERROR);
}
