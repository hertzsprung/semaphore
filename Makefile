CC := clang
PYTEST := python3.3 -m pytest -s
FIND := find
GDB := gdb

MAINDIR := main
TESTDIR := test
UNITTESTDIR := $(TESTDIR)/unit

SRCDIR := src
SRCMAINDIR := $(SRCDIR)/$(MAINDIR)
SRCTESTDIR := $(SRCDIR)/$(TESTDIR)
SRCUNITTESTDIR := $(SRCDIR)/$(UNITTESTDIR)
SRCACCEPTANCETESTDIR := $(SRCTESTDIR)/acceptance

OBJDIR := build
OBJMAINDIR := $(OBJDIR)/$(MAINDIR)
OBJUNITTESTDIR := $(OBJDIR)/$(UNITTESTDIR)

DEPDIR := .dependencies
DEPMAINDIR := $(DEPDIR)/$(MAINDIR)
DEPUNITTESTDIR := $(DEPDIR)/$(UNITTESTDIR)
DEP_MAIN = $(DEPMAINDIR)/$(*F).P
DEP_UNITTEST = $(DEPUNITTESTDIR)/$(*F).P

SRC_UNITTEST := $(shell $(FIND) $(SRCUNITTESTDIR) -type f -name *.c)
OBJ_UNITTEST := $(patsubst $(SRCUNITTESTDIR)/%,$(OBJUNITTESTDIR)/%,$(SRC_UNITTEST:.c=.o))
DEPS_UNITTEST := $(patsubst $(SRCUNITTESTDIR)/%,$(DEPUNITTESTDIR)/%,$(SRC_UNITTEST:.c=.P))

EXECUTABLE := semaphore
SRC_EXECUTABLE := $(SRCMAINDIR)/$(EXECUTABLE).c
OBJ_EXECUTABLE := $(OBJMAINDIR)/$(EXECUTABLE).o

SRC_MAIN := $(shell $(FIND) $(SRCMAINDIR) -type f -name *.c ! -name $(EXECUTABLE).c)
OBJ_MAIN := $(patsubst $(SRCMAINDIR)/%,$(OBJMAINDIR)/%,$(SRC_MAIN:.c=.o))
DEPS_MAIN := $(patsubst $(SRCMAINDIR)/%,$(DEPMAINDIR)/%,$(patsubst %.c,%.P,$(SRC_MAIN) $(SRC_EXECUTABLE)))

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
CAIRO_CFLAGS := $(shell pkg-config cairo --cflags)
CAIRO_LDFLAGS := $(shell pkg-config cairo --libs)
WARNINGS := -pedantic-errors -Werror -Weverything -Wno-error=padded
CFLAGS := $(WARNINGS) -O0 -g $(SDL_CFLAGS) $(CAIRO_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(CAIRO_LDFLAGS)

.DEFAULT_GOAL := all
.PHONY: clean all check unit-test acceptance-test

clean:
	$(RM) -r $(OBJDIR) $(DEPDIR) $(EXECUTABLE)

all: check

check: unit-test acceptance-test

unit-test: $(OBJUNITTESTDIR)/semaphore-unit-test
ifdef DEBUG_UNITTEST
	$(GDB) ./$<
else
	./$<
endif

$(OBJUNITTESTDIR)/semaphore-unit-test: $(OBJ_UNITTEST) $(OBJ_MAIN)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(OBJ_UNITTEST) $(OBJ_MAIN) $(LDFLAGS)

$(OBJ_UNITTEST): $(OBJUNITTESTDIR)/%.o: $(SRCUNITTESTDIR)/%.c | $(OBJUNITTESTDIR) $(DEPUNITTESTDIR)
	$(CC) -MMD -MF $(DEP_UNITTEST) -c $(CPPFLAGS) $(CFLAGS) -I$(SRCMAINDIR) $< -o $@

$(OBJ_MAIN) $(OBJ_EXECUTABLE): $(OBJMAINDIR)/%.o: $(SRCMAINDIR)/%.c | $(OBJMAINDIR) $(DEPMAINDIR)
	$(CC) -MMD -MF $(DEP_MAIN) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

acceptance-test: $(EXECUTABLE)
	$(PYTEST) $(SRCACCEPTANCETESTDIR)

$(EXECUTABLE): $(OBJ_EXECUTABLE) $(OBJ_MAIN)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(OBJ_MAIN) $(LDFLAGS)

$(OBJMAINDIR) $(OBJUNITTESTDIR) $(DEPUNITTESTDIR) $(DEPMAINDIR):
	mkdir -p $@

-include $(DEPS_MAIN) $(DEPS_UNITTEST)
