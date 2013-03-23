#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"

static void next_token_returns_null_for_empty_string();
static void next_token_returns_whole_string_with_no_delimiters();
static void second_next_token();
static void next_token_ignores_multiple_spaces();

int main() {
	next_token_returns_null_for_empty_string();
	next_token_returns_whole_string_with_no_delimiters();
	second_next_token();
	next_token_ignores_multiple_spaces();
	return EXIT_SUCCESS;
}

static void next_token_returns_null_for_empty_string() {
	char input[] = "";
	Tokenization* tokenization = tokenize(input);
	assert(next_token(tokenization) == NULL);
	free(tokenization);
}

static void next_token_returns_whole_string_with_no_delimiters() {
	char input[] = "foo";
	Tokenization* tokenization = tokenize(input);
	assert(strcmp(next_token(tokenization), "foo") == 0);
	free(tokenization);
}

static void second_next_token() {
	char input[] = "foo   bar";
	Tokenization* tokenization = tokenize(input);
	next_token(tokenization);
	assert(strcmp(next_token(tokenization), "bar") == 0);
	free(tokenization);
}

static void next_token_ignores_multiple_spaces() {
	char input[] = " foo   bar  ";
	Tokenization* tokenization = tokenize(input);
	next_token(tokenization);
	assert(strcmp(next_token(tokenization), "bar") == 0);
	free(tokenization);
}
