#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "strings.h"

uint8_t command_read_from(const FILE* const in, Command* const command) {
	char* const line = read_line(in);

	if (line) {
		Tokenization* const tokenization = tokenize(line);
		const char* token = next_token(tokenization);
		if (strcmp(token, "SCREENSHOT") == 0) {
			command->type = SCREENSHOT;
			if ((token = next_token(tokenization))) {
				command->screenshot.filename = token;
			}
		}
		free(tokenization);
		free(line);
		return 1;
	} else {
		return 0;
	}
}

