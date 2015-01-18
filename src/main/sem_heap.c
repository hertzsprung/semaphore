#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_heap.h"
#include "sem_error.h"

int sem_heap_entry_array_init(sem_heap_entry_array* array) {
	array->size = 1;
	array->tail_idx = 0;
	array->entries = malloc(sizeof(sem_heap_entry*)); // TODO: check return
	return SEM_OK;
}

void sem_heap_entry_array_destroy(sem_heap_entry_array* array) {
	free(array->entries);
}

int sem_heap_entry_array_add(sem_heap_entry_array* array, sem_heap_entry* entry) {
	if (array->tail_idx == array->size) {
		array->size *= 2;
		array->entries = realloc(array->entries, sizeof(sem_heap_entry*) * array->size); // TODO: check return
	}

	array->entries[array->tail_idx++] = entry;
	return SEM_OK;
}

int sem_heap_entry_array_remove_last(sem_heap_entry_array* array) {
	if (array->tail_idx == 0) {
		sem_set_error("sem_heap_entry_array already empty");
		return SEM_ERROR;
	}

	array->tail_idx--;
	return SEM_OK;
}

typedef struct sem_heap sem_heap;

struct sem_heap {
	uint32_t size;
	sem_heap_entry** entries;
};

int sem_heap_insert(sem_heap* heap, sem_heap_entry* e);

int sem_heap_insert(sem_heap* heap, sem_heap_entry* e) {
	printf("%d %ld\n", heap->size, e->time);
	return SEM_OK;
}
