MAKEFLAGS += --no-builtin-rules
.DELETE_ON_ERROR:
.SUFFIXES:

CC := clang
DOXYGEN := doxygen
GTESTER := gtester
GTESTER_REPORT := gtester-report
LD := ld
SDL_CONFIG := sdl2-config
PKG_CONFIG := pkg-config

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags | sed -re "s/-I/-isystem /g")
SDL_LDFLAGS := $(shell $(SDL_CONFIG) --libs)
CAIRO_CFLAGS := $(shell $(PKG_CONFIG) cairo --cflags | sed -re "s/-I/-isystem /g")
CAIRO_LDFLAGS := $(shell $(PKG_CONFIG) cairo --libs)
GLIB_CFLAGS := $(shell $(PKG_CONFIG) glib-2.0 --cflags | sed -re "s/-I/-isystem /g")
GLIB_LDFLAGS := $(shell $(PKG_CONFIG) glib-2.0 --libs)
WARNINGS := -pedantic-errors -Werror -Weverything \
-Wno-error=padded -Wno-error=documentation -Wno-error=documentation-unknown-command \
-Wno-format-nonliteral -Wno-padded
COMMON_CFLAGS := $(WARNINGS) -O0 -g -std=gnu99
CFLAGS := $(COMMON_CFLAGS) $(SDL_CFLAGS) $(CAIRO_CFLAGS) $(GLIB_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(CAIRO_LDFLAGS) $(GLIB_LDFLAGS) -lrt -lm -luuid
TEST_CFLAGS := -Isrc/main $(COMMON_CFLAGS) $(GLIB_CFLAGS)
TEST_LDFLAGS := $(GLIB_LDFLAGS) -luuid

COMPONENTS := sem_action sem_action_list sem_compass sem_dynamic_array sem_error sem_heap sem_input sem_render sem_serialize sem_serialize_actions sem_serialize_railpro sem_signal sem_strings sem_parser sem_timer sem_track_cache sem_train sem_world
EXECUTABLES := semaphore railpro2semaphore
SOURCES := $(addsuffix .c,$(addprefix src/main/,$(EXECUTABLES) $(COMPONENTS)))

.DEFAULT_GOAL := all
.PHONY: all clean doc test

include make/Makefile-c
include make/Makefile-main
include make/Makefile-test

all: test build/main/semaphore build/main/railpro2semaphore

doc: Doxyfile $(SOURCES) | build
	$(DOXYGEN)

clean:
	rm -rf build

build:
	mkdir -p $@
