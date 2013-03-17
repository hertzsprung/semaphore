CC = clang
PYTEST = python3.3 -m pytest -s
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)
CAIRO_CFLAGS = $(shell pkg-config cairo --cflags)
CAIRO_LDFLAGS = $(shell pkg-config cairo --libs)
LDFLAGS = $(SDL_LDFLAGS) $(CAIRO_LDFLAGS)
WARNINGS = -pedantic-errors -Werror -Weverything
CFLAGS = -std=c99 $(WARNINGS) -O0 -g $(SDL_CFLAGS) $(CAIRO_CFLAGS)

BIN = bin

all: check

clean:
	$(RM) -r $(BIN)

check: compile
	$(PYTEST) src/test/acceptance

compile:
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) src/main/semaphore.c -o $(BIN)/semaphore $(LDFLAGS)
