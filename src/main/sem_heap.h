#ifndef _SEM_HEAP_H
#define _SEM_HEAP_H

#include <inttypes.h>

#include "sem_error.h"

typedef struct sem_heap sem_heap;

typedef struct sem_heap_entry {
	uint64_t time;
	void* context;
	sem_success (*action)(sem_heap* heap, void* context);
} sem_heap_entry;

/** \brief A dynamic array of sem_heap_entry pointers
 */
struct sem_heap {
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
};


sem_success sem_heap_init(sem_heap* array);

void sem_heap_destroy(sem_heap* array);

sem_success sem_heap_insert(sem_heap* heap, sem_heap_entry* entry);

sem_heap_entry* sem_heap_remove_earliest(sem_heap* heap);

#endif
