#include <stdio.h>
#include <stdlib.h>
#include "command.h"

Command* command_read_from(const FILE* const in) {
	char* line = NULL;
	size_t len = 0;
	if (getline(&line, &len, (FILE*) in) != -1) {
		Command* command = malloc(sizeof(Command));
		free(line);
		return command;
	} else {
		return NULL;
	}
}
