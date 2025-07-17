#include "http_library.hpp"

int http_library::getobjects(lua_State* L)
{
	lua_getglobal(L, "game");
	lua_getfield(L, -1, "GetService");
	lua_pushvalue(L, -2);
	lua_pushstring(L, "InsertService");
	lua_call(L, 2, 1);

	lua_getfield(L, -1, "LoadLocalAsset");
	lua_pushvalue(L, -2);
	lua_pushstring(L, lua_tostring(L, 2));
	lua_call(L, 2, 1);

	lua_newtable(L);
	lua_pushvalue(L, -2);
	lua_rawseti(L, -2, 1);
	return 1;
}

int http_library::httpget(lua_State* L)
{
	std::string url;
	if (!lua_isstring(L, 1)) {
		luaL_checkstring(L, 2);
		url = lua_tostring(L, 2);
	}
	else {
		url = lua_tostring(L, 1);
	}

	if (url.find("http://") != 0 && url.find("https://") != 0) {
		luaL_argerror(L, 1, "Invalid protocol (expected 'http://' or 'https://')");
	}

	std::optional<std::string> job_id;
	lua_getglobal(L, "game");
	lua_getfield(L, -1, "JobId");
	if (lua_isstring(L, -1))
		job_id = lua_tostring(L, -1);
	lua_pop(L, 2);

	uintptr_t place_id = 0;
	lua_getglobal(L, "game");
	lua_getfield(L, -1, "PlaceId");
	if (lua_isnumber(L, -1))
		place_id = static_cast<uintptr_t>(lua_tointeger(L, -1));
	lua_pop(L, 2);

	return yielder::yield_execution(L, [url, job_id, place_id]() -> yielder::yield_return {
		return [url, job_id, place_id](lua_State* L) -> int {
			std::string session_id_header;
			if (job_id.has_value()) {
				session_id_header = "{\"GameId\":\"" + job_id.value() + "\",\"PlaceId\":\"" + std::to_string(place_id) + "\"}";
			}
			else {
				session_id_header = "{\"GameId\":\"empty value\",\"PlaceId\":\"empty value\"}";
			}

			HINTERNET h_internet = InternetOpenA("Roblox/WinInet", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
			if (!h_internet) {
				lua_pushstring(L, "HttpGet failed: InternetOpenA failed");
				return 1;
			}

			HINTERNET h_url = InternetOpenUrlA(h_internet, url.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0);
			if (!h_url) {
				InternetCloseHandle(h_internet);
				lua_pushstring(L, "HttpGet failed: InternetOpenUrlA failed");
				return 1;
			}

			std::string response;
			char buffer[4096];
			DWORD bytes_read = 0;

			while (InternetReadFile(h_url, buffer, sizeof(buffer), &bytes_read) && bytes_read > 0)
				response.append(buffer, bytes_read);

			InternetCloseHandle(h_url);
			InternetCloseHandle(h_internet);

			if (response.empty()) {
				lua_pushstring(L, "HttpGet failed: empty response");
				return 1;
			}

			lua_pushlstring(L, response.data(), response.size());
			return 1;
			};
		});
}

void http_library::initialize(lua_State* L)
{
	register_env_functions(L,
		{
			{"httpget", httpget},
			{"getobjects", getobjects},
			{nullptr, nullptr}
		});
}