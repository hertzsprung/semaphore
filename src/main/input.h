#ifndef _SEMAPHORE_INPUT_H
#define _SEMAPHORE_INPUT_H

#include "types.h"

typedef enum InputType {
	KEY_DOWN,
	KEY_UP
} InputType;

typedef union Input {
	InputType type;
} Input;

Status input_simulate(Input* input);
/* TODO input_destroy(Input* input); */

#endif
