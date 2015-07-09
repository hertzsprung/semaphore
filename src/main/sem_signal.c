#include "sem_signal.h"

sem_success sem_signal_accept(sem_train* train, sem_signal* signal) {
	#pragma unused(train)

	signal->aspect = RED;
	return SEM_OK;
}
