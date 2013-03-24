#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "command.h"
#include "command_test.h"

static void reads_screenshot_command(void);
static void reads_invalid_command(void);
static void reads_invalid_screenshot_command(void);
static void reads_key_command(void);
static void converts_key_command_to_input(void);

static Command* read_command_from(char* input);
static FILE* as_file(char* string);

void command_tests(void) {
	reads_screenshot_command();
	reads_invalid_command();
	reads_invalid_screenshot_command();
	reads_key_command();
	converts_key_command_to_input();
}

static void reads_screenshot_command(void) {
	char input[] = "SCREENSHOT /path/to/myfile.png";
	Command* command = read_command_from(input);
	assert(command->type == SCREENSHOT);
	assert(command->screenshot.filename);
	assert(strcmp(command->screenshot.filename, "/path/to/myfile.png") == 0);
	command_destroy(command);
}

static void reads_invalid_command(void) {
	char input[] = "RUBBISH";
	Command* command = read_command_from(input);
	assert(command->type == INVALID);
	command_destroy(command);
}

static void reads_invalid_screenshot_command(void) {
	char input[] = "SCREENSHOT  ";
	Command* command = read_command_from(input);
	assert(command->type == INVALID);
	command_destroy(command);
}

static void reads_key_command(void) {
	char input[] = "KEY";
	Command* command = read_command_from(input);
	assert(command->type == KEY);
	command_destroy(command);
}

static void converts_key_command_to_input(void) {
	Command* command = malloc(sizeof(Command));
	command->type = KEY;
	Input* input = command_to_input(command);
	assert(input);
	free(input);
	command_destroy(command);
}

static Command* read_command_from(char* input) {
	FILE* const in = as_file(input);
	Command* command;
	assert(command = command_read_from(in));
	fclose(in);
	return command;
}

static FILE* as_file(char* input) {
	return fmemopen(input, strlen(input)+1, "r");
}
