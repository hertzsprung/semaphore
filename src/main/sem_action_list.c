#include "sem_action_list.h"

#include "sem_error.h"
#include "sem_heap.h"

sem_success sem_action_list_execute(sem_heap* heap, uint64_t end_time) {
	if (heap->entries[1]->time <= end_time) {
		sem_heap_entry* entry = sem_heap_remove_earliest(heap);
		entry->action(heap, entry->context); // TODO: check return
	}
	return SEM_OK;
}
