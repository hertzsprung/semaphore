#include <stdlib.h>

#include "sem_action.h"

sem_action* sem_action_new(sem_game* game) {
	sem_action* action = malloc(sizeof(sem_action));
	if (action == NULL) {
		sem_set_error("Could not create action");
		return NULL;
	}
	action->write = NULL;
	action->dynamically_allocated = true;
	action->game = game;
	return action;
}
