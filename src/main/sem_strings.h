#ifndef _SEM_STRINGS_H
#define _SEM_STRINGS_H

#include <inttypes.h>
#include <stdio.h>

typedef struct {
    char* string;
    char* state;
} sem_tokenization;

/**
 * @return the next line in the file, or NULL if the end of the file has been reached.
 *         The caller is responsible for freeing the returned string.
 */
char* sem_read_line(FILE* in);

void sem_tokenization_init(sem_tokenization* tokenization, char* string);

char* sem_tokenization_next(sem_tokenization* tokenization);

char* sem_tokenization_split(sem_tokenization* tokenization, char* delimiter);

uint32_t sem_parse_uint32_t(char* string);

#endif
