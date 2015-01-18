#ifndef _SEM_TIMER_H
#define _SEM_TIMER_H

#include <inttypes.h>

typedef struct {
	uint64_t now; /**< Current game time */
	double multiplier; /**< How game time increases in relation to real life */
	uint64_t clock_now; /**< Current time in real life */
	uint64_t clock_previous;
	int (*clock)(uint64_t* now); /**< a monotonic clock function such as sem_clock_monotonic() */
} sem_timer_context;

int sem_timer_init(sem_timer_context* ctx);
int sem_timer_now(sem_timer_context* ctx);

/**
 * \brief Returns the current monotonic time in milliseconds.
 * 
 * This is not related to the current wall clock time.
 */
int sem_clock_monotonic(uint64_t* now);

#endif
