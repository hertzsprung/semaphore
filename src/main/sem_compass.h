#ifndef _SEM_COMPASS_H
#define _SEM_COMPASS_H

#define SEM_EAST 1
#define SEM_WEST 3
#define SEM_NORTH (3 << 2)
#define SEM_SOUTH (1 << 2)

#include <inttypes.h>

typedef uint8_t unit_vector;

#define SEM_COMPASS_X(v) (((v & 3) ^ 2) - 2)
#define SEM_COMPASS_Y(v) (((v >> 2) ^ 2) - 2)

typedef struct {
	uint32_t x, y;
} sem_coordinate;

void sem_coordinate_set(sem_coordinate* coord, uint32_t x, uint32_t y);

unit_vector sem_compass_opposite_of(unit_vector v);

#endif