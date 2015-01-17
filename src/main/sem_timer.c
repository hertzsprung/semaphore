#include <inttypes.h>

#include "sem_error.h"
#include "sem_timer.h"

int sem_timer_init(sem_timer_context* ctx) {
	return ctx->clock(&(ctx->clock_now));
}

int sem_timer_now(sem_timer_context* ctx) {
	ctx->clock_previous = ctx->clock_now;
	if (sem_timer_init(ctx) != SEM_OK) return SEM_ERROR;
	uint64_t delta = ctx->clock_now - ctx->clock_previous;
	ctx->now += delta * ctx->multiplier;
	return SEM_OK;
}
