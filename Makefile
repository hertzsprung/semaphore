LUA = lua
TEST_PATH = "src/main/?.lua;src/test/?.lua;lib/main/?.lua;lib/test/?.lua"

test:
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-compass.lua