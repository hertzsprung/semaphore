#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "strings.h"

uint8_t command_read_from(const FILE* const in, Command* const command) {
	char* const line = read_line(in);

	if (line) {
		Tokenization* const tokenization = tokenize(line);
		char* token;
		while ((token = next_token(tokenization))) {
			printf("%s", token);
		}
		command->type = SCREENSHOT;
		free(tokenization);
		free(line);
		return 1;
	} else {
		return 0;
	}
}

