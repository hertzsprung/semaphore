#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

typedef struct Tokenization {
	char* string;
	char* state;
} Tokenization;

static char* read_line(const FILE* const in);
static Tokenization* tokenize(char* const string);
static char* next_token(Tokenization* const tokenization);

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

static char* read_line(const FILE* const in) {
	char* line = NULL;
	size_t len = 0;
	if (getline(&line, &len, (FILE*) in) != -1) {
		line[strlen(line)-1] = '\0';
		return line;
	} else {
		return NULL;
	}
}

static Tokenization* tokenize(char* const string) {
	Tokenization* const tokenization = malloc(sizeof(Tokenization));
	tokenization->string = string;
	tokenization->state = NULL;
	return tokenization;
}

static char* next_token(Tokenization* const tokenization) {
	const char* const delimiter = " ";
	char* const token = strtok_r(tokenization->string, delimiter, &tokenization->state);
	tokenization->string = NULL;
	return token;
}
