#include "sem_train.h"

#include "sem_error.h"

sem_success sem_train_move(sem_train* train) {
	/* sign extension */
	train->x += ((train->velocity & 3) ^ 2) - 2;
	train->y += ((train->velocity >> 2) ^ 2) - 2;
	return SEM_OK;
}
