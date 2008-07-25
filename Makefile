LUA = lua
LIB_PATH = "lib/main/?.so"
TEST_PATH = "src/main/?.lua;src/test/?.lua;lib/main/?.lua;lib/test/?.lua;"
LUADOC = luadoc
CC = gcc

.PHONY: test doc libs
libs:
	$(CC) -shared -fPIC lib/main/time.c -o lib/main/time.so `pkg-config --cflags lua5.1`

test: libs
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-action.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-compass.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-map.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-maths.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-signal.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-station.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-tile.lua
	LUA_PATH=$(TEST_PATH) LUA_CPATH=$(LIB_PATH) $(LUA) src/test/test-time.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-train.lua

doc:
	luadoc -d doc/html/ src/main/*.lua