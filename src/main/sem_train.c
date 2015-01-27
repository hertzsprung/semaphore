#include "sem_train.h"

#include "sem_compass.h"
#include "sem_error.h"

sem_success sem_train_move(sem_train* train) {
	train->x += SEM_COMPASS_X(train->velocity);
	train->y += SEM_COMPASS_Y(train->velocity);
	return SEM_OK;
}
