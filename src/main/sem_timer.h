#ifndef SEM_TIMER_H
#define SEM_TIMER_H

#include <inttypes.h>

#include "sem_error.h"

typedef struct {
	uint64_t now; /**< Current game time */
	double multiplier; /**< How game time increases in relation to real life */
	uint64_t clock_now; /**< Current time in real life */
	uint64_t clock_previous;
	sem_success (*clock)(uint64_t* now); /**< a monotonic clock function such as sem_clock_monotonic() */
} sem_timer_context;

/**
 * \brief Initialse the sem_timer_context with the current time as 0 and a multiplier of 1.
 */
sem_success sem_timer_init_default(sem_timer_context* ctx);

sem_success sem_timer_init(sem_timer_context* ctx);
sem_success sem_timer_now(sem_timer_context* ctx);

/**
 * \brief Returns the current monotonic time in milliseconds.
 * 
 * This is not related to the current wall clock time.
 */
sem_success sem_clock_monotonic(uint64_t* now);

#endif
