#include <stdbool.h>
#include <glib.h>

#include "test_action_list.h"

#include "test_heap.h"
#include "sem_action_list.h"
#include "sem_error.h"
#include "sem_heap.h"

void test_action_list_one_old_action_executed(sem_heap* heap, const void* data);
void test_action_list_does_not_execute_future_action(sem_heap* heap, const void* data);

int test_action(sem_heap* heap, void* context);

void add_tests_action_list() {
	add_test_heap("/action_list/one_old_action_executed", test_action_list_one_old_action_executed);
	add_test_heap("/action_list/does_not_execute_future_action", test_action_list_does_not_execute_future_action);
}

int test_action(sem_heap* heap, void* context) {
	#pragma unused(heap)
	*((bool*) context) = true;

	return SEM_OK;
}

void test_action_list_one_old_action_executed(sem_heap* heap, const void* data) {
	#pragma unused(data)

	bool action_called = false;

	sem_heap_entry action;
	action.time = 1000L;
	action.context = &action_called;
	action.action = test_action;

	sem_heap_insert(heap, &action);
	sem_action_list_execute(heap, 2000L); // TODO: check return

	g_assert_true(action_called);
}

void test_action_list_does_not_execute_future_action(sem_heap* heap, const void* data) {
	#pragma unused(data)

	bool action_called = false;

	sem_heap_entry action;
	action.time = 2050L;
	action.context = &action_called;
	action.action = test_action;

	sem_heap_insert(heap, &action);
	sem_action_list_execute(heap, 2000L); // TODO: check return

	g_assert_false(action_called);
}
