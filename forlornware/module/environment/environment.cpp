#include "environment.hpp"

namespace ForlornWare
{
	namespace Yielder
	{
		using YieldReturn = std::function<int(lua_State* L)>;

		struct TaskData
		{
			lua_State* State;
			std::function<YieldReturn()> Generator;
			PTP_WORK Work;
		};

		inline VOID CALLBACK ThreadWorker(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
		{
			auto* Data = static_cast<TaskData*>(Context);

			try
			{
				auto YieldResult = Data->Generator();
				int resultCount = YieldResult(Data->State);

				lua_State* ThreadCtx = lua_newthread(Data->State);

				lua_getglobal(ThreadCtx, "task");
				lua_getfield(ThreadCtx, -1, "defer");
				lua_pushthread(Data->State);
				lua_xmove(Data->State, ThreadCtx, 1);
				lua_pop(Data->State, 1);

				for (int i = resultCount; i >= 1; --i)
				{
					lua_pushvalue(Data->State, -i);
					lua_xmove(Data->State, ThreadCtx, 1);
				}

				lua_pcall(ThreadCtx, resultCount + 1, 0, 0);
				lua_settop(ThreadCtx, 0);
			}
			catch (const std::exception&)
			{
			}

			CloseThreadpoolWork(Data->Work);
			delete Data;
		}

		inline int YieldExecution(lua_State* L, const std::function<YieldReturn()>& Generator)
		{
			lua_pushthread(L);
			lua_ref(L, -1);
			lua_pop(L, 1);

			auto* Task = new TaskData{ L, Generator, nullptr };

			Task->Work = CreateThreadpoolWork(ThreadWorker, Task, nullptr);
			if (Task->Work)
			{
				SubmitThreadpoolWork(Task->Work);
			}
			else
			{
				delete Task;
			}

			L->base = L->top;
			L->status = LUA_YIELD;
			L->ci->flags |= 1;
			return -1;
		}
	}
}

namespace env
{
	namespace http
	{
		int getobjects(lua_State* L) {
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

		int httpget(lua_State* L)
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
				luaL_argerror(L, 1, "Invalid protocol(expected 'http://' or 'https://')");
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
				place_id = (uintptr_t)(lua_tointeger(L, -1));
			lua_pop(L, 2);

			return ForlornWare::Yielder::YieldExecution(L, [url, job_id, place_id]() -> ForlornWare::Yielder::YieldReturn {
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
						lua_pushstring(L, "why does this nigger shit fail bro wtf");
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
						lua_pushstring(L, "failed :((((( you are gay");
						return 1;
					}

					lua_pushlstring(L, response.data(), response.size());
					return 1;
					};
				});
		}
	}

    namespace test
    {
        int identifyexecutor(lua_State* L)
        {
            lua_pushstring(L, "ForlornWare");
            lua_pushstring(L, "1.0.0");
            return 2;
        }

        int getgenv(lua_State* L) {
            lua_pushvalue(L, LUA_ENVIRONINDEX);
            return 1;
        }

        int loadstring(lua_State* L)
        {
            luaL_checktype(L, 1, LUA_TSTRING);

            const char* source = lua_tostring(L, 1);
            const char* chunkname = luaL_optstring(L, 2, "ForlornWare");

            std::string bytecode = compile_script(source);

            if (luau_load(L, chunkname, bytecode.c_str(), bytecode.size(), 0) != LUA_OK)
            {
                lua_pushnil(L);
                lua_pushvalue(L, -2);
                return 2;
            }

            if (Closure* func = lua_toclosure(L, -1))
            {
                if (func->l.p)
                    task_scheduler::set_proto_capabilities(func->l.p, &max_caps);
            }

            lua_setsafeenv(L, LUA_GLOBALSINDEX, false);
            return 1;
        }

        static const luaL_Reg functions[] = {
            {"identifyexecutor", identifyexecutor},
			{"httpget", http::httpget},
			{"getgenv", getgenv},
			{"loadstring", loadstring},
            {nullptr, nullptr}
        };
    }

	namespace name_call
	{
		static __int64 old_namecall;
		static __int64 old_index;

		auto namecall_hook(lua_State* L) -> int
		{
			const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50);

			if (L->namecall && !script_ptr)
			{
				const char* data = L->namecall->data;

				if (!strcmp(data, "HttpGet") || !strcmp(data, "HttpGetAsync"))
				{
					return http::httpget(L);
				}

				if (!strcmp(data, "GetObjects") || !strcmp(data, "GetObjectsAsync"))
				{
					return http::getobjects(L);
				}
			}

			return static_cast<int>(((__int64(__fastcall*)(__int64))old_namecall)((__int64)L));
		}

		auto index_hook(lua_State* L) -> int
		{
			auto state = (__int64)L;
			const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50);

			uintptr_t userdata = *(uintptr_t*)(state + 0x78);
			int level = static_cast<int>(*(uintptr_t*)(userdata + 0x30));

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

		void initialize(lua_State* L)
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
	}
}

void environment::initialize(lua_State* L)
{
    register_env_functions(L, env::test::functions);

	env::name_call::initialize(L);

    lua_newtable(L);
    lua_setglobal(L, "_G");

    lua_newtable(L);
    lua_setglobal(L, "shared");
}

// not the best code heh