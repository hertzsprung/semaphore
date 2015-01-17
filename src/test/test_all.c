#include <stdio.h>
#include <inttypes.h>

#include <glib.h>

#include "sem_timer.h"

void test_timer_provides_initial_time(void);

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/timer_provides_initial_time", test_timer_provides_initial_time);

	return g_test_run();
}

void test_timer_provides_initial_time() {
	uint64_t now = 123456789012345L;
	sem_timer_context timer_ctx;
	timer_ctx.now = now;
	
	g_assert_cmpuint(sem_timer_now(&timer_ctx), ==, now);
}

