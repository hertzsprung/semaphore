#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "strings.h"

Command* command_read_from(const FILE* const in) {
	char* const line = read_line(in);
	if (!line) {
		return NULL;
	}

	Command* command = malloc(sizeof(Command));
	Tokenization* const tokenization = tokenize(line);
	const char* token = next_token(tokenization);
	if (strcmp(token, "SCREENSHOT") == 0) {
		if ((token = next_token(tokenization))) {
			command->type = SCREENSHOT;
			command->screenshot.filename = strdup(token);
		} else {
			command->type = INVALID;
		}
	} else if (strcmp(token, "KEY") == 0) {
		command->type = KEY;
	} else {
		command->type = INVALID;
	}

	free(tokenization);
	free(line);
	return command;
}

Input* command_to_input(Command* command) {
	Input* input = malloc(sizeof(Input));
	printf("%u", command->type);
	return input;
}

void command_destroy(Command* command) {
	if (command->type == SCREENSHOT) free((void*)command->screenshot.filename);
	free(command);
}
