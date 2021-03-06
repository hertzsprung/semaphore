#include <stdlib.h>

#include "sem_action_list.h"

#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"

sem_success sem_action_list_execute(sem_dynamic_array* heap, uint64_t end_time) {
	while (heap->tail_idx > 1 && ((sem_action*) heap->items[1])->time <= end_time) {
		sem_action* action = sem_heap_remove_earliest(heap);
		action->destroyable = true;
		sem_success success = action->function(heap, action);
		if (action->dynamically_allocated && action->destroyable) {
			free(action);
		}
		if (success != SEM_OK) {
			return SEM_ERROR;
		}
	}
	return SEM_OK;
}

