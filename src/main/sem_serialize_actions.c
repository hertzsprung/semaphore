#include <stdio.h>

#include "sem_action.h"
#include "sem_serialize_actions.h"

sem_action_reader sem_action_reader_lookup(char* action_name) {
	#pragma unused(action_name)
	return sem_remove_train_action_reader;
}

sem_success sem_remove_train_action_reader(FILE* in, sem_action** action) {
	#pragma unused(in)
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	return SEM_OK;
}

sem_success sem_remove_train_action_write(FILE* out, sem_action* action) {
	#pragma unused(action)
	fprintf(out, "remove_train");

	return SEM_OK;
}
