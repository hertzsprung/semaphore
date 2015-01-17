#ifndef _SEM_TIMER_H
#define _SEM_TIMER_H

#include <inttypes.h>

typedef struct {
	uint64_t now; /**< current game time */
	double multiplier; /**< how game time increases in relation to real life */
	uint64_t clock_now; /**< current time in real life */
	uint64_t clock_previous;
	int (*clock)(uint64_t* now);
} sem_timer_context;

int sem_timer_init(sem_timer_context* ctx);
int sem_timer_now(sem_timer_context* ctx);

#endif
