#ifndef _SEM_ERROR_H
#define _SEM_ERROR_H

#define SEM_OK 0
#define SEM_ERROR -1

#define SEM_ERROR_BUF_SIZE 1024

char* sem_get_error(void);
void sem_set_error(const char* format, ...);

/**
 * \brief Prints the current error message to `stderr` and returns `EXIT_FAILURE`
 */
int sem_fatal_error(void);

#endif
