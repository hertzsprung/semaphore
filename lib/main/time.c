/*
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Contributed by Rob Kendrick
*/

#include <lua.h>
#include <lauxlib.h>
#include <sys/time.h>
#include <time.h>
 
static int L_gettimeofday(lua_State *L)
{
        struct timeval tv;
        struct timezone tz;
 
        gettimeofday(&tv, &tz);
 
        lua_pushnumber(L, tv.tv_sec);
        lua_pushnumber(L, tv.tv_usec);
 
        lua_pushnumber(L, tz.tz_minuteswest);
        lua_pushnumber(L, tz.tz_dsttime);
 
        return 4;
}
 
static const struct luaL_Reg reg_gettimeofday[] = {
        { "gettimeofday",       L_gettimeofday },
 
        { NULL, NULL }
};
 
int luaopen_ltime(lua_State *L)
{
        luaL_register(L, "ltime", reg_gettimeofday);
 
        return 1;
}
