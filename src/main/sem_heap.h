#ifndef _SEM_HEAP_H
#define _SEM_HEAP_H

#include <inttypes.h>

typedef struct sem_heap_entry {
	uint64_t time;
	//void* action_context;
	//int (*action)(sem_heap* heap, void* context);
} sem_heap_entry;

typedef struct {
	uint32_t size; /**< Size of array (which may be larger than the number of entries) */
	uint32_t tail_idx; /**< Index into entries of next element.  This can also be treated as the logical size of the array. */
	sem_heap_entry** entries;
} sem_heap_entry_array;

int sem_heap_entry_array_init(sem_heap_entry_array* array);

void sem_heap_entry_array_destroy(sem_heap_entry_array* array);

int sem_heap_entry_array_add(sem_heap_entry_array* array, sem_heap_entry* entry);

int sem_heap_entry_array_remove_last(sem_heap_entry_array* array);

#endif
