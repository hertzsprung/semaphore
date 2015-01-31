#include "sem_train.h"

#include "sem_compass.h"
#include "sem_error.h"
#include "sem_world.h"

sem_success sem_train_move(sem_train* train) {
	train->x += SEM_COMPASS_X(train->direction);
	train->y += SEM_COMPASS_Y(train->direction);

	sem_tile* tile = sem_tile_at(train->world, train->x, train->y);
	return sem_tile_redirect(train, tile); // TODO: check return
}
