#include "sem_compass.h"

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
