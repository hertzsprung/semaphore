#ifndef _SEMAPHORE_COMMAND_H
#define _SEMAPHORE_COMMAND_H

#include <stdio.h>
#include "input.h"

typedef enum CommandType {
	INVALID,
	SCREENSHOT,
	KEY
} CommandType;

typedef struct ScreenshotCommand {
	CommandType type;
	const char* filename;
} ScreenshotCommand;

typedef struct KeyCommand {
	CommandType type;
} KeyCommand;

typedef union Command {
	CommandType type;
	ScreenshotCommand screenshot;
	KeyCommand key;
} Command;

Command* command_read_from(const FILE* const in);
Input* command_to_input(Command* command);
void command_destroy(Command* command);

#endif
