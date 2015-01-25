#include <inttypes.h>
#include <stdlib.h>

#include "sem_heap.h"
#include "sem_error.h"

sem_success sem_heap_extend_tail(sem_heap* heap);

void sem_heap_percolate_down(sem_heap* heap, uint32_t hole);

sem_success sem_heap_init(sem_heap* heap) {
	heap->size = 1;
	heap->tail_idx = 1;
	heap->entries = malloc(sizeof(sem_action*));
	if (heap->entries == NULL) {
		return sem_set_error("Failed to malloc heap entries");
	}
	return SEM_OK;
}

void sem_heap_destroy(sem_heap* heap) {
	free(heap->entries);
}

sem_success sem_heap_extend_tail(sem_heap* heap) {
	if (heap->tail_idx == heap->size) {
		heap->size *= 2;
		heap->entries = realloc(heap->entries, sizeof(sem_action*) * heap->size);
		if (heap->entries == NULL) {
			return sem_set_error("Failed to increase size of heap");
		}
	}

	heap->tail_idx++;
	return SEM_OK;
}

sem_success sem_heap_insert(sem_heap* heap, sem_action* e) {
	uint32_t hole = heap->tail_idx;
	if (sem_heap_extend_tail(heap) == SEM_ERROR) return SEM_ERROR;
	for (; hole > 1 && e->time < heap->entries[hole/2]->time; hole /= 2) {
		heap->entries[hole] = heap->entries[hole/2];
	}
	heap->entries[hole] = e;
	return SEM_OK;
}

sem_action* sem_heap_remove_earliest(sem_heap* heap) {
	if (heap->tail_idx == 1) {
		return NULL;
	}

	sem_action* e = heap->entries[1];
	heap->entries[1] = heap->entries[heap->tail_idx-1];
	heap->tail_idx--;
	sem_heap_percolate_down(heap, 1);
	return e;
}

void sem_heap_percolate_down(sem_heap* heap, uint32_t hole) {
	uint32_t child;
	sem_action* tmp = heap->entries[hole];
	for (; hole * 2 <= heap->tail_idx-1; hole = child) {
		child = hole * 2;
		if (child != heap->tail_idx-1 && heap->entries[child+1]->time < heap->entries[child]->time) {
			child++;
		}
		if (heap->entries[child]->time < tmp->time) {
			heap->entries[hole] = heap->entries[child];
		} else {
			break;
		}
	}
	heap->entries[hole] = tmp;
}
