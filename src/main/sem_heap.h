#ifndef SEM_HEAP_H
#define SEM_HEAP_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "sem_dynamic_array.h"
#include "sem_error.h"

typedef struct sem_action sem_action;

typedef sem_success (*sem_action_func)(sem_dynamic_array* heap, sem_action* action);
typedef sem_success (*sem_write_action_func)(FILE* out, sem_action* action);

struct sem_action {
	uint64_t time;
	void* context;
	sem_action_func function;
	sem_write_action_func write;
	bool dynamically_allocated; /**< If true, the action will be freed by sem_action_list_execute() after it is executed */
	bool destroyable; /**< If true, the action will be destroyed after its function has been executed.  If false, the action will be reinserted onto the heap. */
};

sem_success sem_heap_init(sem_dynamic_array* heap);

sem_success sem_heap_insert(sem_dynamic_array* heap, sem_action* entry);

/**
 * \return The \ref sem_action with the earliest time, or NULL if the heap is empty.
 */
sem_action* sem_heap_remove_earliest(sem_dynamic_array* heap);

uint32_t sem_heap_size(sem_dynamic_array* heap);

sem_action* sem_heap_peek(sem_dynamic_array* heap, uint32_t index);

#endif
