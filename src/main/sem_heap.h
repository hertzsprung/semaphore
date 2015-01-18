#ifndef _SEM_HEAP_H
#define _SEM_HEAP_H

#include <inttypes.h>

typedef struct sem_heap_entry {
	uint64_t time;
	//void* action_context;
	//int (*action)(sem_heap* heap, void* context);
} sem_heap_entry;

/** \brief A dynamic array of sem_heap_entry pointers
 */
typedef struct {
	/** \brief Physical size of the array.
	 *
	 * This may be larger than the number of entries
	 */
	uint32_t size;

	/** \brief Index into entries of next element.
	 *
	 * This can also be treated as the logical size of the array.
	 */
	uint32_t tail_idx;

	sem_heap_entry** entries;
} sem_heap;


int sem_heap_init(sem_heap* array);

void sem_heap_destroy(sem_heap* array);

int sem_heap_insert(sem_heap* heap, sem_heap_entry* entry);

sem_heap_entry* sem_heap_remove_min(sem_heap* heap);

#endif
