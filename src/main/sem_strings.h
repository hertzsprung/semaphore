#ifndef _SEM_STRINGS_H
#define _SEM_STRINGS_H

#include <stdio.h>

typedef struct {
    char* string;
    char* state;
} sem_tokenization;

char* sem_read_line(FILE* in);
void sem_tokenization_init(sem_tokenization* tokenization, char* string);
char* sem_tokenization_next(sem_tokenization* tokenization);

#endif
