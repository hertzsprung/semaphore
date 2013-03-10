CC = clang
PYTEST = python3.3 -m pytest -s
CFLAGS = -std=c99 -pedantic-errors -Werror -Weverything
BIN = bin

all: check

clean:
	$(RM) -r $(BIN)

check: compile
	$(PYTEST) src/test/acceptance

compile:
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) src/main/semaphore.c -o $(BIN)/semaphore
