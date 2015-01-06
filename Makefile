MAKEFLAGS += --no-builtin-rules
.DELETE_ON_ERROR:
.SUFFIXES:

CC := clang
LD := ld

.DEFAULT: build/semaphore
.PHONY: clean

build/semaphore: build/semaphore.o | build
	$(CC) -o $@ $<

build/semaphore.o: src/semaphore.c | build
	$(CC) -o $@ -c $<

build:
	mkdir -p $@

clean:
	rm -rf build
