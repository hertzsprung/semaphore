#include <glib.h>

#include "test_dynamic_array.h"
#include "sem_dynamic_array.h"

void add_test_dynamic_array(const char *test_name, void (*test)(sem_dynamic_array* a, const void* data));
void test_dynamic_array_setup(sem_dynamic_array* a, const void* data);
void test_dynamic_array_teardown(sem_dynamic_array* a, const void* data);

void test_dynamic_array_add_item(sem_dynamic_array* a, const void* data);

void add_tests_dynamic_array() {
	add_test_dynamic_array("/dynamic_array/add_item", test_dynamic_array_add_item);
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
