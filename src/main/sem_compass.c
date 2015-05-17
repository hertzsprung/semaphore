#include <math.h>

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

bool sem_compass_straight(unit_vector a, unit_vector b) {
	return sem_compass_opposite_of(a) == b;
}

unit_vector sem_compass_corner_of_curve(unit_vector a, unit_vector b) {
	unit_vector existing_corner;
	unit_vector non_corner;

	if (sem_compass_is_corner(a)) {
		existing_corner = a;
		non_corner = b;
	} else {
		existing_corner = b;
		non_corner = a;
	}

	unit_vector opposite_corner = sem_compass_opposite_of(existing_corner);
	if (SEM_COMPASS_X(opposite_corner) == SEM_COMPASS_X(non_corner)) {
		return (non_corner & SEM_COMPASS_HORIZONTAL_MASK) | (existing_corner & SEM_COMPASS_VERTICAL_MASK);
	} else {
		return (non_corner & SEM_COMPASS_VERTICAL_MASK) | (existing_corner & SEM_COMPASS_HORIZONTAL_MASK);
	}
}

bool sem_compass_is_corner(unit_vector v) {
	return SEM_COMPASS_X(v) != 0 && SEM_COMPASS_Y(v) != 0;
}

double sem_compass_rotation(unit_vector v) {
	if (SEM_COMPASS_X(v) == 0) {
		return 0.5 * M_PI;
	} else if (SEM_COMPASS_Y(v) == 0) {
		return 0;
	} else if (SEM_COMPASS_X(v) < 0 && SEM_COMPASS_Y(v) < 0) {
		return 0.25 * M_PI;
	} else if (SEM_COMPASS_X(v) < 0 && SEM_COMPASS_Y(v) > 0) {
		return -0.25 * M_PI;
	} else if (SEM_COMPASS_X(v) > 0 && SEM_COMPASS_Y(v) < 0) {
		return -0.25 * M_PI;
	} else {
		return 0.25 * M_PI;
	}
}
