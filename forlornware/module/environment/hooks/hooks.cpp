#include "hooks.hpp"

static __int64 old_namecall;
static __int64 old_index;

auto namecall_hook(lua_State* L) -> int
{
	const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50); // script offset

	if (L->namecall && !script_ptr)
	{
		const char* data = L->namecall->data;

		if (!strcmp(data, "HttpGet") || !strcmp(data, "HttpGetAsync"))
		{
			return http_library::httpget(L);
		}

		if (!strcmp(data, "GetObjects") || !strcmp(data, "GetObjectsAsync"))
		{
			return http_library::getobjects(L);
		}
	}

	return static_cast<int>(((__int64(__fastcall*)(__int64))old_namecall)((__int64)L));
}

auto index_hook(lua_State* L) -> int
{
	auto state = (__int64)L;
	const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50); // script offset

	uintptr_t userdata = *(uintptr_t*)(state + 0x78); // userdata offset
	int level = static_cast<int>(*(uintptr_t*)(userdata + 0x30)); // identity offset

	if (lua_isstring(L, 2) && !script_ptr)
	{
		const char* data = luaL_checkstring(L, 2);

		if (!strcmp(data, "HttpGet") || !strcmp(data, "HttpGetAsync"))
		{
			lua_getglobal(L, "httpget");
			return 1;
		}

		if (!strcmp(data, "GetObjects") || !strcmp(data, "GetObjectsAsync"))
		{
			lua_getglobal(L, "getobjects");
			return 1;
		}
	}

	return static_cast<int>(((__int64(__fastcall*)(__int64))old_index)((__int64)L));
}

void hooks::initialize(lua_State* L)
{
	lua_getglobal(L, "game");
	lua_getmetatable(L, -1);
	lua_getfield(L, -1, "__namecall");

	Closure* namecall = (Closure*)lua_topointer(L, -1);
	lua_CFunction namecall_f = namecall->c.f;
	old_namecall = (__int64)namecall_f;
	namecall->c.f = namecall_hook;

	lua_settop(L, 0);

	lua_getglobal(L, "game");
	lua_getmetatable(L, -1);
	lua_getfield(L, -1, "__index");

	Closure* index = (Closure*)lua_topointer(L, -1);
	lua_CFunction index_f = index->c.f;
	old_index = (__int64)index_f;
	index->c.f = index_hook;
}