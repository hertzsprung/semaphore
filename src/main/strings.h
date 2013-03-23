#ifndef _SEMAPHORE_STRINGS_H
#define _SEMAPHORE_STRINGS_H

#include <stdio.h>

typedef struct Tokenization {
	char* string;
	char* state;
} Tokenization;

char* read_line(const FILE* const in);
Tokenization* tokenize(char* const string);
char* next_token(Tokenization* const tokenization);

#endif
