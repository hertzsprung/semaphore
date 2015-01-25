#ifndef _SEM_TRAIN_H
#define _SEM_TRAIN_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	bool moving;
	uint32_t x, y;
} sem_train;

#endif
