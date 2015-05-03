#ifndef SEM_ACTION_LIST_H
#define SEM_ACTION_LIST_H

#include <inttypes.h>

#include "sem_dynamic_array.h"
#include "sem_error.h"

/**
 * \brief Execute all actions at or before \a end_time in chronological order.
 */
sem_success sem_action_list_execute(sem_dynamic_array* heap, uint64_t end_time);

#endif
