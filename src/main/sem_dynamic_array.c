#include <stdlib.h>

#include "sem_dynamic_array.h"

#include "sem_error.h"

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
	// initially size = 1, tail = 0
	if (array->tail_idx == array->size) {
		 // TODO: resize
	}
	array->items[array->tail_idx] = item;
	return SEM_OK;
}

void sem_dynamic_array_destroy(sem_dynamic_array* array) {
	free(array->items);
}
