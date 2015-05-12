#ifndef SEM_SERIALIZE_ACTIONS_H
#define SEM_SERIALIZE_ACTIONS_H

#include "sem_error.h"
#include "sem_heap.h"

typedef sem_success (*sem_action_reader)(FILE* in, sem_action** action);

sem_action_reader sem_action_reader_lookup(char* action_name);

sem_success sem_remove_train_action_reader(FILE* in, sem_action** action);

sem_success sem_remove_train_action_write(FILE* out, sem_action* action);

#endif
