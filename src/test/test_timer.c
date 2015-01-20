#include <glib.h>

#include "test_timer.h"

#include "sem_error.h"
#include "sem_timer.h"

sem_success test_fixed_clock(uint64_t* now);
sem_success test_fixed_increment_clock(uint64_t* now);

sem_success test_fixed_clock(uint64_t* now) {
	*now = 9876543L;
	return SEM_OK;
}

sem_success test_fixed_increment_clock(uint64_t* now) {
	static uint64_t next = 100000L;
	next += 1000L;
	*now = next;
	return SEM_OK;
}

void test_timer_increments_by_one_second() {
	uint64_t now = 123456789012345L;
	uint64_t one_second_later = 123456789013345L;

	sem_timer_context timer_ctx;
	timer_ctx.now = now;
	timer_ctx.multiplier = 1.0;
	timer_ctx.clock = test_fixed_increment_clock;

	sem_timer_init(&timer_ctx);
	sem_timer_now(&timer_ctx);

	g_assert_cmpuint(timer_ctx.now, ==, one_second_later);
}

void test_timer_increments_twice_clock_with_multiple_of_two() {
	uint64_t now = 123456789012345L;
	uint64_t two_seconds_later = 123456789014345L;

	sem_timer_context timer_ctx;
	timer_ctx.now = now;
	timer_ctx.multiplier = 2.0;
	timer_ctx.clock = test_fixed_increment_clock;

	sem_timer_init(&timer_ctx);
	sem_timer_now(&timer_ctx);

	g_assert_cmpuint(timer_ctx.now, ==, two_seconds_later);
}

