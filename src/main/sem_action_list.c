#include <stdlib.h>

#include "sem_action_list.h"

#include "sem_error.h"
#include "sem_heap.h"

sem_success sem_action_list_execute(sem_heap* heap, uint64_t end_time) {
	while (heap->tail_idx > 1 && heap->entries[1]->time <= end_time) {
		sem_action* entry = sem_heap_remove_earliest(heap);
		sem_success success = entry->function(heap, entry);
		if (entry->dynamically_allocated) {
			free(entry);
		}
		if (success != SEM_OK) {
			return SEM_ERROR;
		}
	}
	return SEM_OK;
}
