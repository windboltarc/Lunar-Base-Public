#include "script_library.hpp"

int identifyexecutor(lua_State* L)
{
	lua_pushstring(L, "ForlornWare");
	lua_pushstring(L, "1.0.0");
	return 2;
}

int getgenv(lua_State* L)
{
	lua_pushvalue(L, LUA_ENVIRONINDEX);
	return 1;
}

void script_library::initialize(lua_State* L)
{
	register_env_functions(L,
		{
			{"identifyexecutor", identifyexecutor},
			{"getgenv", getgenv},
			{nullptr, nullptr}
		});
}