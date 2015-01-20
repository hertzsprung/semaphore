#include <inttypes.h>
#include <stdbool.h>
#include <glib.h>

#include "test_action_list.h"

#include "test_heap.h"
#include "sem_action_list.h"
#include "sem_error.h"
#include "sem_heap.h"

void test_action_list_two_old_actions_executed(sem_heap* heap, const void* data);
void test_action_list_does_not_execute_future_action(sem_heap* heap, const void* data);
void test_action_list_does_nothing_for_empty_list(sem_heap* heap, const void* data);

int test_action(sem_heap* heap, sem_action* action);

void add_tests_action_list() {
	add_test_heap("/action_list/two_old_action_executed", test_action_list_two_old_actions_executed);
	add_test_heap("/action_list/does_not_execute_future_action", test_action_list_does_not_execute_future_action);
	add_test_heap("/action_list/does_nothing_for_empty_list", test_action_list_does_nothing_for_empty_list);
}

int test_action(sem_heap* heap, sem_action* action) {
	#pragma unused(heap)
	(*((uint32_t*) action->context))++;

	return SEM_OK;
}

void test_action_list_two_old_actions_executed(sem_heap* heap, const void* data) {
	#pragma unused(data)

	uint32_t action_called = 0;

	sem_action a1, a2;
	a1.time = 1000L;
	a1.context = &action_called;
	a1.function = test_action;

	a2.time = 2000L;
	a2.context = &action_called;
	a2.function = test_action;

	sem_heap_insert(heap, &a1);
	sem_heap_insert(heap, &a2);
	sem_action_list_execute(heap, 2000L); // TODO: check return

	g_assert_cmpuint(action_called, ==, 2);
}

void test_action_list_does_not_execute_future_action(sem_heap* heap, const void* data) {
	#pragma unused(data)

	uint32_t action_called = 0;

	sem_action action;
	action.time = 2050L;
	action.context = &action_called;
	action.function= test_action;

	sem_heap_insert(heap, &action);
	sem_action_list_execute(heap, 2000L); // TODO: check return

	g_assert_cmpuint(action_called, ==, 0);
}

void test_action_list_does_nothing_for_empty_list(sem_heap* heap, const void* data) {
	#pragma unused(data)

	sem_action_list_execute(heap, 2000L); // TODO: check return
}
