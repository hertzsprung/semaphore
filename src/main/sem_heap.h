#ifndef _SEM_HEAP_H
#define _SEM_HEAP_H

#include <inttypes.h>
#include <stdbool.h>

#include "sem_error.h"

typedef struct sem_heap sem_heap;

typedef struct sem_action sem_action;

typedef sem_success (*sem_action_func)(sem_heap* heap, sem_action* action);

struct sem_action {
	uint64_t time;
	void* context;
	sem_action_func function;
	bool dynamically_allocated; /**< If true, the action will be freed by sem_action_list_execute() after it is executed */
};

/** \brief A dynamic array of sem_action pointers
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

	sem_action** entries;
};


sem_success sem_heap_init(sem_heap* heap);

void sem_heap_destroy(sem_heap* heap);

sem_success sem_heap_insert(sem_heap* heap, sem_action* entry);

sem_action* sem_heap_remove_earliest(sem_heap* heap);

#endif
