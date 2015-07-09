#include <stdio.h>
#include <string.h>

#include "sem_action.h"
#include "sem_input.h"
#include "sem_serialize_actions.h"
#include "sem_world.h"

sem_success sem_move_train_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action);
sem_success sem_remove_train_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action);
sem_success sem_reverse_train_at_buffer_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action);

sem_success write_train_action(FILE* out, char* tag, sem_action* action);
sem_success set_train_context(sem_world* world, sem_tokenization* tokens, sem_action* action);

sem_action_reader sem_action_reader_lookup(char* action_name) {
	if (strcmp(action_name, "move_train") == 0) {
		return sem_move_train_action_reader;
	} else if (strcmp(action_name, "remove_train") == 0) {
		return sem_remove_train_action_reader;
	} else if (strcmp(action_name, "reverse_train_at_buffer") == 0) {
		return sem_reverse_train_at_buffer_action_reader;
	} else {
		return NULL;
	}
}

sem_success sem_move_train_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action) {
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	(*action)->function = sem_move_train_action; 
	(*action)->write = sem_move_train_action_write;

	return set_train_context(world, tokens, *action);
}

sem_success sem_remove_train_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action) {
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	(*action)->function = remove_train_action; 
	(*action)->write = sem_remove_train_action_write;

	return set_train_context(world, tokens, *action);
}

sem_success sem_reverse_train_at_buffer_action_reader(sem_tokenization* tokens, sem_world* world, sem_action** action) {
	*action = sem_action_new();
	if (*action == NULL) return SEM_ERROR;
	(*action)->function = sem_reverse_train_at_buffer_action; 
	(*action)->write = sem_reverse_train_at_buffer_action_write;

	return set_train_context(world, tokens, *action);
}

sem_success sem_move_train_action_write(FILE* out, sem_action* action) {
	return write_train_action(out, "move_train", action);
}

sem_success sem_remove_train_action_write(FILE* out, sem_action* action) {
	return write_train_action(out, "remove_train", action);
}

sem_success sem_reverse_train_at_buffer_action_write(FILE* out, sem_action* action) {
	return write_train_action(out, "reverse_train_at_buffer", action);
}

sem_success write_train_action(FILE* out, char* tag, sem_action* action) {
	char id_str[37];
	uuid_unparse(((sem_train*) action->context)->id, id_str);
	fprintf(out, "%s %s", tag, id_str);

	return SEM_OK;
}

sem_success set_train_context(sem_world* world, sem_tokenization* tokens, sem_action* action) {
	char* train_id_str = sem_tokenization_next(tokens);
	uuid_t train_id;
	uuid_parse(train_id_str, train_id);
	sem_train* train = sem_train_by_id(world, train_id);
	if (train == NULL) return sem_set_error("Cannot remove train with unknown id");

	action->context = train;
	return SEM_OK;
}
