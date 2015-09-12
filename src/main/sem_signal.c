#include <stdbool.h>
#include <stdio.h>

#include "sem_error.h"
#include "sem_signal.h"

void sem_signal_init(sem_signal* signal, sem_signal_type type, sem_signal_aspect aspect) {
	signal->type = type;
	signal->aspect = aspect;
	signal->previous_aspect = aspect;
	signal->held_train = NULL;
}

void sem_signal_set_aspect(sem_signal* signal, sem_signal_aspect aspect) {
	signal->previous_aspect = signal->aspect;
	signal->aspect = aspect;
}

void sem_signal_force_aspect(sem_signal* signal, sem_signal_aspect aspect) {
	signal->aspect = aspect;
	signal->previous_aspect = aspect;
}

void sem_signal_set_previous_aspect(sem_signal* signal) {
	sem_signal_set_aspect(signal, signal->previous_aspect);
}

sem_success sem_signal_accept(sem_train* train, sem_signal* signal, sem_signal_acceptance* acceptance) {
	if (signal->aspect == RED) {
		if (signal->type == SUB && train->speed == FAST) {
			acceptance->change_speed = true;
			acceptance->speed = SLOW;
		} else {
			acceptance->stop = true;
			signal->held_train = train;
		}
	} else if (signal->aspect == AMBER) {
		acceptance->change_speed = true;
		acceptance->speed = (signal->type == SUB ? MEDIUM : SLOW);
	} else if (signal->aspect == GREEN) {
		acceptance->change_speed = true;
		acceptance->speed = FAST;
	}

	if (signal->type == SUB && signal->aspect != RED) {
		sem_signal_set_aspect(signal, AMBER);
	} else {
		sem_signal_set_aspect(signal, RED);
	}

	if (!acceptance->stop) {
		train->previous_signal = train->signal;
		train->signal = signal;
	}

	return SEM_OK;
}

void sem_signal_portal_exit(sem_train* train) {
	train->previous_signal = train->signal;
	train->signal = NULL;

	if (train->previous_signal != NULL) {
		sem_signal_set_previous_aspect(train->previous_signal);
	}
}

void sem_signal_train_cleared(sem_train* train) {
	if (train->previous_signal != NULL && train->signal->type == SUB) {
		sem_signal_set_previous_aspect(train->previous_signal);
	}
}

void sem_signal_acceptance_init(sem_signal_acceptance* acceptance) {
	acceptance->stop = false;
	acceptance->change_speed = false;
}

bool sem_signal_holding_train(sem_signal* signal) {
	return signal->held_train != NULL;
}

sem_train* sem_signal_release_train(sem_signal* signal) {
	sem_train* t = signal->held_train;
	signal->held_train = NULL;
	return t;
}
