#ifndef _SEM_ACTION_LIST_H
#define _SEM_ACTION_LIST_H

#include <inttypes.h>

#include "sem_error.h"
#include "sem_heap.h"

/**
 * \brief Execute all actions at or before \a end_time in chronological order.
 */
sem_success sem_action_list_execute(sem_heap* heap, uint64_t end_time);

#endif
