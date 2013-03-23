#ifndef _SEMAPHORE_COMMAND_H
#define _SEMAPHORE_COMMAND_H

#include <stdio.h>
#include <stdint.h>

typedef struct ScreenshotCommand {
	const char* const filename;
} ScreenshotCommand;

typedef enum CommandType {
	SCREENSHOT
} CommandType;

typedef union Command {
	CommandType type;
	ScreenshotCommand screenshot;
} Command;

uint8_t command_read_from(const FILE* const in, Command* const command);

#endif
