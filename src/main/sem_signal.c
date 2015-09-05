#include <stdbool.h>
#include <stdio.h>

#include "sem_error.h"
#include "sem_signal.h"

void sem_signal_init(sem_signal* signal, sem_signal_type type, sem_signal_aspect aspect) {
	signal->type = type;
	signal->aspect = aspect;
	signal->held_train = NULL;
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

	signal->aspect = RED;
	return SEM_OK;
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
