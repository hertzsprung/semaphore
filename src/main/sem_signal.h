#ifndef SEM_SIGNAL_H
#define SEM_SIGNAL_H

typedef struct sem_signal sem_signal;

#include <stdbool.h>

#include "sem_error.h"
#include "sem_train.h"

typedef enum {
	MAIN_MANUAL, MAIN_AUTO, SUB
} sem_signal_type;

typedef enum {
	RED, AMBER, GREEN
} sem_signal_aspect;

struct sem_signal {
	sem_signal_type type;
	sem_signal_aspect aspect;
	sem_train* held_train;
};

void sem_signal_init(sem_signal* signal, sem_signal_type type, sem_signal_aspect aspect);
void sem_signal_acceptance_init(sem_signal_acceptance* acceptance);
sem_success sem_signal_accept(sem_train* train, sem_signal* signal, sem_signal_acceptance* acceptance);
bool sem_signal_holding_train(sem_signal* signal);
sem_train* sem_signal_release_train(sem_signal* signal);

#endif

