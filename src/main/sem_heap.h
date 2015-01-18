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

/** \brief Add an entry to the tail of the array.
 *
 * This function does not guarantee the natural ordering of the array.
 * Clients should use sem_heap_insert() to preserve ordering.
 */
int sem_heap_add_tail(sem_heap* array, sem_heap_entry* entry);

/** \brief Remove an entry from the tail of the array.
 *
 * This function does not guarantee the natural ordering of the array.
 * Clients should use sem_heap_remove_min() to preserve ordering.
 */
int sem_heap_remove_tail(sem_heap* array);

#endif
