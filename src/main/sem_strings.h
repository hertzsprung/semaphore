#ifndef SEM_STRINGS_H
#define SEM_STRINGS_H

#include <inttypes.h>
#include <stdio.h>

typedef struct {
    char* string;
    char* state;
    char* delimiter;
} sem_tokenization;

/**
 * @return the next line in the file, or NULL if the end of the file has been reached.
 *         The caller is responsible for freeing the returned string.
 */
char* sem_read_line(FILE* in);

void sem_tokenization_init(sem_tokenization* tokenization, char* string, char* delimiter);

/**
 * @return the next token, or NULL if there are no more tokens
 */
char* sem_tokenization_next(sem_tokenization* tokenization);

uint8_t sem_parse_uint8_t(char* string);

uint32_t sem_parse_uint32_t(char* string);

int32_t sem_parse_int32_t(char* string);

#endif
