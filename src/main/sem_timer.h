#ifndef _SEM_TIMER_H
#define _SEM_TIMER_H

#include <inttypes.h>

typedef struct {
	uint64_t now;
} sem_timer_context;

uint64_t sem_timer_now(sem_timer_context* ctx);

#endif
