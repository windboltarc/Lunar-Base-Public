#include "execution.hpp"

void execution::execute_script(lua_State* l, const std::string& script)
{
    if (script.empty()) return;

    const int original_top = lua_gettop(l);
    auto thread = lua_newthread(l);
    lua_pop(l, 1);

    luaL_sandboxthread(thread);

    auto bytecode = compile_script(script);
    if (luau_load(thread, "@ForlornWare", bytecode.c_str(), bytecode.length(), 0) != LUA_OK)
    {
        const char* err = lua_tostring(thread, -1);
        roblox::r_print(0, "%s", err);
        return;
    }

    Closure* closure = (Closure*)lua_topointer(thread, -1);
    if (closure && closure->l.p)
        context_manager::set_proto_capabilities(closure->l.p, &max_caps);

    lua_getglobal(l, "task");
    lua_getfield(l, -1, "defer");
    lua_remove(l, -2);
    lua_xmove(thread, l, 1);

    if (lua_pcall(l, 1, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(l, -1);
        if (err) roblox::r_print(0, err);
        lua_pop(l, 1);
        return;
    }

    lua_settop(thread, 0);
    lua_settop(l, original_top);
}