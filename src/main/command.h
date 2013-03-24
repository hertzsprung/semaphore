#ifndef _SEMAPHORE_COMMAND_H
#define _SEMAPHORE_COMMAND_H

#include <stdio.h>

typedef enum CommandType {
	INVALID,
	SCREENSHOT
} CommandType;

typedef struct ScreenshotCommand {
	CommandType type;
	const char* filename;
} ScreenshotCommand;

typedef union Command {
	CommandType type;
	ScreenshotCommand screenshot;
} Command;

Command* command_read_from(const FILE* const in);
void command_destroy(Command* command);

#endif
