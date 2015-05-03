#ifndef SEM_DYNAMIC_ARRAY_H
#define SEM_DYNAMIC_ARRAY_H

#include <inttypes.h>

#include "sem_error.h"

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

	void** items;	
} sem_dynamic_array;

sem_success sem_dynamic_array_init(sem_dynamic_array* array);

sem_success sem_dynamic_array_add(sem_dynamic_array* array, void* item);

void sem_dynamic_array_destroy(sem_dynamic_array* array);

#endif

