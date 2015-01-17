#include <inttypes.h>

#include "sem_timer.h"

uint64_t sem_timer_now(sem_timer_context* ctx) {
	return ctx->now;
}
