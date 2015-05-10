#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_heap.h"
#include "sem_error.h"

sem_success sem_heap_extend_tail(sem_dynamic_array* heap);

void sem_heap_percolate_down(sem_dynamic_array* heap, uint32_t hole);

// TODO: this is almost identical to sem_dynamic_array_init except for the tail_idx
sem_success sem_heap_init(sem_dynamic_array* heap) {
	heap->size = 1;
	heap->tail_idx = 1;
	heap->items = malloc(sizeof(sem_action*));
	if (heap->items == NULL) {
		return sem_set_error("Failed to malloc heap items");
	}
	return SEM_OK;
}

sem_success sem_heap_extend_tail(sem_dynamic_array* heap) {
	if (heap->tail_idx == heap->size) {
		heap->size *= 2;
		heap->items = realloc(heap->items, sizeof(sem_action*) * heap->size);
		if (heap->items == NULL) {
			return sem_set_error("Failed to increase size of heap");
		}
	}

	heap->tail_idx++;
	return SEM_OK;
}

sem_success sem_heap_insert(sem_dynamic_array* heap, sem_action* a) {
	a->destroyable = false;	
	uint32_t hole = heap->tail_idx;
	if (sem_heap_extend_tail(heap) == SEM_ERROR) return SEM_ERROR;
	for (; hole > 1 && a->time < ((sem_action*) heap->items[hole/2])->time; hole /= 2) {
		heap->items[hole] = heap->items[hole/2];
	}
	heap->items[hole] = a;
	return SEM_OK;
}

sem_action* sem_heap_remove_earliest(sem_dynamic_array* heap) {
	if (heap->tail_idx == 1) {
		return NULL;
	}

	sem_action* e = heap->items[1];
	heap->items[1] = heap->items[heap->tail_idx-1];
	heap->tail_idx--;
	sem_heap_percolate_down(heap, 1);
	return e;
}

void sem_heap_percolate_down(sem_dynamic_array* heap, uint32_t hole) {
	uint32_t child;
	sem_action* tmp = heap->items[hole];
	for (; hole * 2 <= heap->tail_idx-1; hole = child) {
		child = hole * 2;
		if (child != heap->tail_idx-1 && ((sem_action*) heap->items[child+1])->time < ((sem_action*) heap->items[child])->time) {
			child++;
		}
		if (((sem_action*) heap->items[child])->time < tmp->time) {
			heap->items[hole] = heap->items[child];
		} else {
			break;
		}
	}
	heap->items[hole] = tmp;
}
