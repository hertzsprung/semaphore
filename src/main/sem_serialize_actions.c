#include <stdio.h>
#include <string.h>

#include "sem_action.h"
#include "sem_input.h"
#include "sem_serialize_actions.h"
#include "sem_world.h"

sem_action_reader sem_action_reader_lookup(char* action_name) {
	if (strcmp(action_name, "remove_train") == 0) {
		return sem_remove_train_action_reader;
	} else {
		return NULL;
	}
}

sem_success sem_remove_train_action_reader(FILE* in, sem_world* world, sem_action** action) {
	#pragma unused(in)
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	(*action)->function = remove_train_action; 
	(*action)->context = world->trains->items[0]; // FIXME: lookup train based on its UUID
	(*action)->write = sem_remove_train_action_write;
	return SEM_OK;
}

sem_success sem_remove_train_action_write(FILE* out, sem_action* action) {
	#pragma unused(action)
	fprintf(out, "remove_train");

	return SEM_OK;
}
