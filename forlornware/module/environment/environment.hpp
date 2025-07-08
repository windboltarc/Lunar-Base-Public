#pragma once
#include "Windows.h"
#include "iostream"
#include "vector"
#include <string>
#include <vector>
#include <algorithm>
#include <map>      
#include <cstdlib>   
#include <stdexcept> 
#include <ctime>    
#include <cctype>    
#include <random>    
#include <sstream>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include "update.hpp"
#include "misc/globals.hpp"
#include "../roblox/task_scheduler/task_scheduler.hpp"

#include "VM/include/lua.h"
#include "VM/include/lualib.h"
#include "VM/src/lcommon.h"
#include "VM/src/lstring.h"
#include "VM/src/lfunc.h"
#include "VM/src/lmem.h"
#include "VM/src/lgc.h"
#include "VM/src/ltable.h"
#include "VM/src/lobject.h"
#include "VM/src/lstate.h"
#include "VM/src/lapi.h"
#include "VM/src/ldo.h"

#include "zstd/include/zstd/zstd.h"
#include "zstd/include/zstd/xxhash.h"
#include "Compiler/include/Luau/Compiler.h"
#include "Compiler/include/luacode.h"
#include "Common/include/Luau/BytecodeUtils.h"
#include "Compiler/include/Luau/BytecodeBuilder.h"

inline void register_env_functions(lua_State* l, const luaL_Reg* functions) {
    lua_pushvalue(l, LUA_GLOBALSINDEX);
    luaL_register(l, nullptr, functions);
    lua_pop(l, 1);
}

inline void register_env_members(lua_State* l, const luaL_Reg* functions, const std::string& global_name) {
    luaL_register(l, global_name.c_str(), functions);
}

inline void register_to_global(lua_State* l, const luaL_Reg* functions, const std::string& global_name) {
    lua_getglobal(l, global_name.c_str());
    if (lua_istable(l, -1)) {
        lua_setreadonly(l, -1, false);
        luaL_register(l, nullptr, functions);
        lua_setreadonly(l, -1, true);
    }
    lua_pop(l, 1);
}

class environment
{
public:
	static void initialize(lua_State* l);
};