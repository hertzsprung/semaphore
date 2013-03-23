#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "command.h"
#include "command_test.h"

static void reads_screenshot_command();

void command_tests(void) {
	reads_screenshot_command();
}

static void reads_screenshot_command() {
	char input[] = "SCREENSHOT /path/to/myfile.png";
	FILE* const in = fmemopen(input, sizeof(input), "r");
	Command* command = malloc(sizeof(Command));
	assert(command_read_from(in, command));
	assert(command->type == SCREENSHOT);
	assert(command->screenshot.filename);
	assert(strcmp(command->screenshot.filename, "/path/to/myfile.png") == 0);
	free(command);
	fclose(in);
}
