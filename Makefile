LUA = lua
LIB_PATH = "lib/main/?.so"
TEST_PATH = "src/main/?.lua;src/test/?.lua;lib/main/?.lua;lib/test/?.lua;"
LUADOC = luadoc
CC = gcc

.PHONY: test doc libs
libs:
	$(CC) -shared -fPIC lib/main/time.c -o lib/main/ltime.so `pkg-config --cflags lua5.1`

test: libs
	LUA_PATH=$(TEST_PATH) LUA_CPATH=$(LIB_PATH) $(LUA) src/test/test-all.lua

doc:
	luadoc -d doc/html/ src/main/*.lua