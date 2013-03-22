#ifndef _SEMAPHORE_COMMAND_H
#define _SEMAPHORE_COMMAND_H

#include <stdio.h>
#include <stdint.h>

typedef struct ScreenshotCommand {
	const char* const filename;
} ScreenshotCommand;

typedef union Command {
	const uint8_t type;
	const ScreenshotCommand screenshot;
} Command;

Command* command_read_from(const FILE* const in);

#endif
