#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "sem_error.h"

char* sem_get_error(void) {
	static char message[SEM_ERROR_BUF_SIZE] = "";
	return message;
}

sem_success sem_set_error(const char* format, ...) {
	va_list arglist;
	char* error = sem_get_error();
	va_start(arglist, format);
	vsnprintf(error, SEM_ERROR_BUF_SIZE * sizeof(char), format, arglist);
	va_end(arglist);
	return SEM_ERROR;
}

int sem_fatal_error(void) {
	fprintf(stderr, "FATAL: %s", sem_get_error());
	return EXIT_FAILURE;
}
