#include "sem_compass.h"

void sem_coordinate_set(sem_coordinate* coord, uint32_t x, uint32_t y) {
	coord->x = x;
	coord->y = y;
}

bool sem_coordinate_equal(sem_coordinate a, sem_coordinate b) {
	return a.x == b.x && a.y == b.y;
}

unit_vector sem_compass_opposite_of(unit_vector v) {
	unit_vector opposite = 0;
	if (SEM_COMPASS_X(v) > 0) {
		opposite |= SEM_WEST;
	} else if (SEM_COMPASS_X(v) < 0) {
		opposite |= SEM_EAST;
	}

	if (SEM_COMPASS_Y(v) > 0) {
		opposite |= SEM_NORTH;
	} else if (SEM_COMPASS_Y(v) < 0) {
		opposite |= SEM_SOUTH;
	}
	return opposite;	
}
