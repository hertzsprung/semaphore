#ifndef SEM_SIGNAL_H
#define SEM_SIGNAL_H

#include <uuid/uuid.h>

typedef struct sem_signal sem_signal;
typedef uuid_t sem_signal_id;

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
	sem_signal_id id;
	sem_signal_type type;
	sem_signal_aspect aspect;
	sem_signal_aspect previous_aspect;
	sem_train* held_train;
};

void sem_signal_init(sem_signal* signal, sem_signal_type type, sem_signal_aspect aspect);
void sem_signal_acceptance_init(sem_signal_acceptance* acceptance);
void sem_signal_set_aspect(sem_signal* signal, sem_signal_aspect aspect);
void sem_signal_force_aspect(sem_signal* signal, sem_signal_aspect aspect);
void sem_signal_set_previous_aspect(sem_signal* signal);
sem_success sem_signal_accept(sem_train* train, sem_signal* signal, sem_signal_acceptance* acceptance);
void sem_signal_train_cleared(sem_train* train, sem_signal* signal);
void sem_signal_portal_exit(sem_train* train);
bool sem_signal_holding_train(sem_signal* signal);
sem_train* sem_signal_release_train(sem_signal* signal);

#endif

