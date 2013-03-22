CC := clang
PYTEST := python3.3 -m pytest -s
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
CAIRO_CFLAGS := $(shell pkg-config cairo --cflags)
CAIRO_LDFLAGS := $(shell pkg-config cairo --libs)
LDFLAGS := $(SDL_LDFLAGS) $(CAIRO_LDFLAGS) -lreadline
#-Wno-error=padded needed for SDL.h
WARNINGS := -pedantic-errors -Werror -Weverything -Wno-error=padded
CFLAGS := $(WARNINGS) -O0 -g $(SDL_CFLAGS) $(CAIRO_CFLAGS)
OBJDIR := bin
EXECUTABLE := $(OBJDIR)/semaphore
OBJECTS := $(addprefix $(OBJDIR)/,semaphore.o command.o)

vpath %.c src/main
vpath %.o $(OBJDIR)

.PHONY: clean all check
.DEFAULT_GOAL := all

clean:
	$(RM) -r $(OBJDIR)

all: check

check: $(EXECUTABLE)
	$(PYTEST) src/test/acceptance

$(EXECUTABLE): $(OBJECTS) | $(OBJDIR)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

$(OBJDIR)/semaphore.o: semaphore.c command.o | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(OBJDIR)/command.o: command.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(OBJDIR):
	mkdir -p $@
