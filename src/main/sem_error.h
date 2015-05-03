#ifndef SEM_ERROR_H
#define SEM_ERROR_H

/** \brief Indicates a function completed without error */
#define SEM_OK 0

/**
 * \brief Indicates a function did not complete successfully because of an error.
 *
 * Call sem_get_error() to get the cause of the error.
 */
#define SEM_ERROR -1

#define SEM_ERROR_BUF_SIZE 1024

/**
 * \brief Functions that return SEM_OK or SEM_ERROR should use this return type.
 */
typedef int sem_success;
char* sem_get_error(void);

/**
 * \return Always returns SEM_ERROR.
 */
sem_success sem_set_error(const char* format, ...);

/**
 * \brief Prints the current error message to `stderr` and returns `EXIT_FAILURE`
 */
int sem_fatal_error(void);

#endif
