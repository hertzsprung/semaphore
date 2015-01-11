MAKEFLAGS += --no-builtin-rules
.DELETE_ON_ERROR:
.SUFFIXES:

CC := clang
DOXYGEN := doxygen
LD := ld
SDL_CONFIG := sdl2-config
PKG_CONFIG := pkg-config

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LDFLAGS := $(shell $(SDL_CONFIG) --libs)
CAIRO_CFLAGS := $(shell $(PKG_CONFIG) cairo --cflags)
CAIRO_LDFLAGS := $(shell $(PKG_CONFIG) cairo --libs)
WARNINGS := -pedantic-errors -Werror -Weverything \
-Wno-error=padded -Wno-error=documentation -Wno-error=documentation-unknown-command \
-Wno-format-nonliteral
CFLAGS := $(WARNINGS) -O0 -g $(SDL_CFLAGS) $(CAIRO_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(CAIRO_LDFLAGS)

COMPONENTS := semaphore sem_error
SOURCES := $(addsuffix .c,$(addprefix src/main/,$(COMPONENTS)))
OBJECTS := $(addsuffix .o,$(addprefix build/main/,$(COMPONENTS)))

.DEFAULT_GOAL := build/main/semaphore
.PHONY: clean doc

include make/Makefile-c

$(eval $(call OBJ,sem_error))
$(eval $(call OBJ,semaphore))
$(eval $(call DEP,semaphore,sem_error))

build/main/semaphore: $(OBJECTS) | build/main
	$(CC) -o $@ $^ $(LDFLAGS)

doc: Doxyfile $(SOURCES) | build
	$(DOXYGEN)

clean:
	rm -rf build

build/main:
	mkdir -p $@

build:
	mkdir -p $@
