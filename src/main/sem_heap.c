#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_heap.h"
#include "sem_error.h"

int sem_heap_init(sem_heap* heap) {
	heap->size = 1;
	heap->tail_idx = 0;
	heap->entries = malloc(sizeof(sem_heap_entry*)); // TODO: check return
	return SEM_OK;
}

void sem_heap_destroy(sem_heap* heap) {
	free(heap->entries);
}

int sem_heap_add_tail(sem_heap* heap, sem_heap_entry* entry) {
	if (heap->tail_idx == heap->size) {
		heap->size *= 2;
		heap->entries = realloc(heap->entries, sizeof(sem_heap_entry*) * heap->size); // TODO: check return
	}

	heap->entries[heap->tail_idx++] = entry;
	return SEM_OK;
}

int sem_heap_remove_tail(sem_heap* heap) {
	if (heap->tail_idx == 0) {
		sem_set_error("sem_heap already empty");
		return SEM_ERROR;
	}

	heap->tail_idx--;
	return SEM_OK;
}

int sem_heap_insert(sem_heap* heap, sem_heap_entry* e) {
	printf("%d %ld\n", heap->size, e->time);
	return SEM_OK;
}
