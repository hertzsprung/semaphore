LUA = lua
TEST_PATH = "src/main/?.lua;src/test/?.lua;lib/main/?.lua;lib/test/?.lua"
LUADOC = luadoc

.PHONY: test doc
test:
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-action.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-compass.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-map.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-maths.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-signal.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-tile.lua
	LUA_PATH=$(TEST_PATH) $(LUA) src/test/test-train.lua

doc:
	luadoc -d doc/html/ src/main/*.lua