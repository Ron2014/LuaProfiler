#include <stdint.h>
#include <string.h>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

int luaopen_pb(lua_State *L);