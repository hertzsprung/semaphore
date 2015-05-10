#include <stdlib.h>

#include "sem_dynamic_array.h"

#include "sem_error.h"

sem_success sem_dynamic_array_extend_tail(sem_dynamic_array* array);
void shift_down(sem_dynamic_array* array, uint32_t index);

sem_success sem_dynamic_array_init(sem_dynamic_array* array) {
	array->size = 1;
	array->tail_idx = 0;
	array->items = malloc(sizeof(void*));
	if (array->items == NULL) {
		return sem_set_error("Failed to malloc array items");
	}
	return SEM_OK;
}

sem_success sem_dynamic_array_add(sem_dynamic_array* array, void* item) {
	if (sem_dynamic_array_extend_tail(array) != SEM_OK) return SEM_ERROR;
	array->items[array->tail_idx-1] = item;
	return SEM_OK;
}

sem_success sem_dynamic_array_remove(sem_dynamic_array* array, void* item) {
	uint32_t i=0;
	for (; i<array->tail_idx; i++) {
		if (array->items[i] == item) break;
	}
	if (i == array->tail_idx) return sem_set_error("Cannot remove item that is not in dynamic array");
	
	shift_down(array, i+1);
	array->tail_idx--;

	return SEM_OK;
}

void shift_down(sem_dynamic_array* array, uint32_t index) {
	for (uint32_t i=index; i<array->tail_idx; i++) {
		array->items[i-1] = array->items[i];
	}
}

void sem_dynamic_array_destroy(sem_dynamic_array* array) {
	free(array->items);
}

sem_success sem_dynamic_array_extend_tail(sem_dynamic_array* array) {
	if (array->tail_idx == array->size) {
		array->size *= 2;
		array->items = realloc(array->items, sizeof(void*) * array->size);
		if (array->items == NULL) {
			return sem_set_error("Failed to increase size of array");
		}
	}

	array->tail_idx++;
	return SEM_OK;
}

