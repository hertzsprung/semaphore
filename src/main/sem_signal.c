#include <stdio.h>

#include "sem_error.h"
#include "sem_signal.h"

sem_success sem_signal_accept(sem_train* train, sem_signal* signal, sem_signal_acceptance* acceptance) {
	#pragma unused(train)

	if (signal->aspect == RED) {
		acceptance->stop = true;
	}

	signal->aspect = RED;
	return SEM_OK;
}

void sem_signal_acceptance_init(sem_signal_acceptance* acceptance) {
	acceptance->stop = false;
}
