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
-Wno-error=padded -Wno-error=documentation -Wno-error=documentation-unknown-command
CFLAGS := $(WARNINGS) -O0 -g $(SDL_CFLAGS) $(CAIRO_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(CAIRO_LDFLAGS)

.DEFAULT: build/main/semaphore
.PHONY: clean doc

build/main/semaphore: build/main/semaphore.o | build/main
	$(CC) -o $@ $< $(LDFLAGS)

build/main/semaphore.o: src/main/semaphore.c | build/main
	$(CC) -o $@ -c $< $(CFLAGS)

doc: Doxyfile src/main/semaphore.c | build
	$(DOXYGEN)

clean:
	rm -rf build

build/main:
	mkdir -p $@

build:
	mkdir -p $@
