#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_strings.h"

char* strip_newline(char* string);

char* sem_read_line(FILE* in) {
	char* line = NULL;
	size_t len = 0;
	if (getline(&line, &len, in) != -1) {
		return strip_newline(line);
	} else {
		free(line);
		return NULL;
	}
}

void sem_tokenization_init(sem_tokenization* tokenization, char* string) {
	tokenization->string = string;
	tokenization->state = NULL;
}

char* sem_tokenization_next(sem_tokenization* tokenization) {
	char* delimiter = " ";
	char* token = strtok_r(tokenization->string, delimiter, &tokenization->state);
	tokenization->string = NULL;
	return token;
}

uint32_t sem_parse_uint32_t(char* string) {
	return (uint32_t) strtoul(string, NULL, 10);
}

char* strip_newline(char* string) {
	if (string[strlen(string)-1] == '\n') string[strlen(string)-1] = '\0';
	return string;
}
