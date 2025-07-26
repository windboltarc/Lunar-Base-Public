#include "closure_library.hpp"

int loadstring(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);

    const char* source = lua_tostring(L, 1);
    const char* chunk_name = luaL_optstring(L, 2, "ForlornWare");

    const std::string& bytecode = global_functions::compile_script(source);

    if (luau_load(L, chunk_name, bytecode.data(), bytecode.size(), 0) != LUA_OK)
    {
        lua_pushnil(L);
        lua_pushvalue(L, -2);
        return 2;
    }

    if (Closure* func = lua_toclosure(L, -1))
    {
        if (func->l.p)
            context_manager::set_proto_capabilities(func->l.p, &max_caps);
    }

    lua_setsafeenv(L, LUA_GLOBALSINDEX, false);
    return 1;
}

void closure_library::initialize(lua_State* L)
{
	register_env_functions(L,
		{
			{"loadstring", loadstring},
			{nullptr, nullptr}
		});
}