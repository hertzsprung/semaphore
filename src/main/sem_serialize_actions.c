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

sem_success sem_remove_train_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action) {
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	(*action)->function = remove_train_action; 
	(*action)->write = sem_remove_train_action_write;

	char* train_id_str = sem_tokenization_next(tokens);
	uuid_t train_id;
	uuid_parse(train_id_str, train_id);
	sem_train* train = sem_train_by_id(world, train_id);
	if (train == NULL) return sem_set_error("Cannot remove train with unknown id");
	(*action)->context = train;
	
	return SEM_OK;
}

sem_success sem_remove_train_action_write(FILE* out, sem_action* action) {
	char id_str[37];
	uuid_unparse(((sem_train*) action->context)->id, id_str);
	fprintf(out, "remove_train %s", id_str);

	return SEM_OK;
}
