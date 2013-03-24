#include <string.h>
#include <stdlib.h>
#include "strings.h"

static char* strip_newline(char* string);

char* read_line(const FILE* const in) {
	char* line = NULL;
	size_t len = 0;
	if (getline(&line, &len, (FILE*) in) != -1) {
		return strip_newline(line);
	} else {
		free(line);
		return NULL;
	}
}

Tokenization* tokenize(char* const string) {
	Tokenization* const tokenization = malloc(sizeof(Tokenization));
	tokenization->string = string;
	tokenization->state = NULL;
	return tokenization;
}

char* next_token(Tokenization* const tokenization) {
	const char* const delimiter = " ";
	char* const token = strtok_r(tokenization->string, delimiter, &tokenization->state);
	tokenization->string = NULL;
	return token;
}

static char* strip_newline(char* string) {
	if (string[strlen(string)-1] == '\n') string[strlen(string)-1] = '\0';
	return string;
}
