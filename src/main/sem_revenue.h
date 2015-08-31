#ifndef SEM_REVENUE_H
#define SEM_REVENUE_H

#include <stdint.h>

typedef struct {
	uint32_t balance;
} sem_revenue;

void sem_revenue_init(sem_revenue* revenue);

#endif
