#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem_heap.h"
#include "sem_error.h"

int sem_heap_extend_tail(sem_heap* heap);

void sem_heap_percolate_down(sem_heap* heap, uint32_t hole);

int sem_heap_init(sem_heap* heap) {
	heap->size = 1;
	heap->tail_idx = 1;
	heap->entries = malloc(sizeof(sem_heap_entry*)); // TODO: check return
	return SEM_OK;
}

void sem_heap_destroy(sem_heap* heap) {
	free(heap->entries);
}

int sem_heap_extend_tail(sem_heap* heap) {
	if (heap->tail_idx == heap->size) {
		heap->size *= 2;
		heap->entries = realloc(heap->entries, sizeof(sem_heap_entry*) * heap->size); // TODO: check return
	}

	heap->tail_idx++;
	return SEM_OK;
}

int sem_heap_insert(sem_heap* heap, sem_heap_entry* e) {
	uint32_t hole = heap->tail_idx;
	sem_heap_extend_tail(heap); // TODO: check return
	for (; hole > 1 && e->time < heap->entries[hole/2]->time; hole /= 2) {
		heap->entries[hole] = heap->entries[hole/2];
	}
	heap->entries[hole] = e;
	return SEM_OK;
}

sem_heap_entry* sem_heap_remove_min(sem_heap* heap) {
	if (heap->tail_idx == 1) {
		return NULL;
	}

	sem_heap_entry* e = heap->entries[1];
	heap->entries[1] = heap->entries[heap->tail_idx-1];
	heap->tail_idx--;
	sem_heap_percolate_down(heap, 1);
	return e;
}

void sem_heap_percolate_down(sem_heap* heap, uint32_t hole) {
	uint32_t child;
	sem_heap_entry* tmp = heap->entries[hole];
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
