#include <stdbool.h>
#include <stdio.h>

#include "sem_error.h"
#include "sem_signal.h"

void signal_update_aspects(sem_train* train, sem_signal* signal);
void signal_update_aspects_after_sub(sem_train* train);
void signal_update_aspects_after_main(sem_train* train);

void sem_signal_init(sem_signal* signal, sem_signal_type type, sem_signal_aspect aspect) {
	uuid_generate(signal->id);
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
			acceptance->signal = signal;
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

	return SEM_OK;
}

void sem_signal_portal_exit(sem_train* train) {
	for (uint32_t i=train->signals->tail_idx; i > 0; i--) {
		sem_signal* signal = (sem_signal*) train->signals->items[i-1];
		sem_signal_set_previous_aspect(signal);
	}
	sem_dynamic_array_remove_all(train->signals);
}

void sem_signal_train_cleared(sem_train* train, sem_signal* signal) {
	signal_update_aspects(train, signal);
	if (signal->type == MAIN_MANUAL) {
		if (train->signals->tail_idx > 0) {
			sem_signal* most_recently_cleared = sem_dynamic_array_tail_item(train->signals);
			if (most_recently_cleared->type == SUB) {
				sem_dynamic_array_remove_at(train->signals, train->signals->tail_idx-1);
			}
		}
	} else {
		sem_dynamic_array_add(train->signals, signal);
	}
}

void signal_update_aspects(sem_train* train, sem_signal* signal) {
	if (signal->type == SUB) {
		signal_update_aspects_after_sub(train);
	} else {
		signal_update_aspects_after_main(train);
	}
}

void signal_update_aspects_after_sub(sem_train* train) {
	for (uint32_t i=train->signals->tail_idx; i > 0; i--) {
		sem_signal* signal = (sem_signal*) train->signals->items[i-1];
		if (signal->type != SUB) break;
		sem_signal_set_previous_aspect(signal);
		sem_dynamic_array_remove_at(train->signals, i-1);
	}
}

void signal_update_aspects_after_main(sem_train* train) {
	uint32_t signals_to_dequeue = 0;

	for (uint32_t i=0; i < train->signals->tail_idx; i++) {
		sem_signal* signal = (sem_signal*) train->signals->items[i];
		if (i == train->signals->tail_idx-1 && signal->type == SUB) break;
		sem_signal_set_previous_aspect(signal);
		signals_to_dequeue++;
	}
	for (uint32_t j=0; j < signals_to_dequeue; j++) {
		sem_dynamic_array_remove_at(train->signals, 0);
	}
}

void sem_signal_acceptance_init(sem_signal_acceptance* acceptance) {
	acceptance->stop = false;
	acceptance->signal = NULL;
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
