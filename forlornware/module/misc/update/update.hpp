#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>
#include <iostream>

#include "enchelper/enchelper.hpp"

struct lua_State;

#define rebase(x) (x + (uintptr_t)(GetModuleHandle(nullptr)))
    
// updated for version-225e87fdb7254f64
namespace update
{
    namespace roblox
    {
        const uintptr_t print = rebase(0x14B9880);
        const uintptr_t luad_throw = rebase(0x2652140);
    }

    namespace lua
    {
        const uintptr_t luao_nilobject = rebase(0x47562F8);
        const uintptr_t luau_execute = rebase(0x2684D00);
        const uintptr_t luah_dummynode = rebase(0x4755D18);
        const uintptr_t opcode_lookup = rebase(0x4D50700);
    }

    namespace offsets
    {
        namespace datamodel
        {
            const uintptr_t fake_datamodel = rebase(0x6833728);
            const uintptr_t fake_datamodel_to_datamodel = 0x1B8;
            const uintptr_t script_context = 0x3C0;
            const uintptr_t game_loaded = 0x660;
        }

        namespace instance
        {
            const uintptr_t name = 0x78;
            const uintptr_t children = 0x80;
        }
    }
}

namespace roblox
{
    using print_func_t = int(__fastcall*)(int, const char*, ...);
    inline print_func_t r_print = reinterpret_cast<print_func_t>(update::roblox::print);

    using luad_throw_t = void(__fastcall*)(lua_State*, int);
    inline luad_throw_t luad_throw = reinterpret_cast<luad_throw_t>(update::roblox::luad_throw);
}

#define LUAU_COMMA_SEP ,
#define LUAU_SEMICOLON_SEP ;

#define LUAU_SHUFFLE3(s, a1, a2, a3) a3 s a2 s a1
#define LUAU_SHUFFLE4(s, a1, a2, a3, a4) a4 s a3 s a1 s a2
#define LUAU_SHUFFLE5(s, a1, a2, a3, a4, a5) a3 s a4 s a2 s a5 s a1
#define LUAU_SHUFFLE6(s, a1, a2, a3, a4, a5, a6) a3 s a1 s a2 s a6 s a4 s a5
#define LUAU_SHUFFLE7(s, a1, a2, a3, a4, a5, a6, a7) a7 s a5 s a6 s a3 s a1 s a2 s a4
#define LUAU_SHUFFLE8(s, a1, a2, a3, a4, a5, a6, a7, a8) a1 s a4 s a5 s a3 s a8 s a2 s a7 s a6
#define LUAU_SHUFFLE9(s, a1, a2, a3, a4, a5, a6, a7, a8, a9) a9 s a2 s a4 s a8 s a3 s a7 s a5 s a1 s a6

#define PROTO_MEMBER1_ENC VMValue0
#define PROTO_MEMBER2_ENC VMValue2
#define PROTO_DEBUGISN_ENC VMValue4
#define PROTO_TYPEINFO_ENC VMValue1
#define PROTO_DEBUGNAME_ENC VMValue3

#define LSTATE_STACKSIZE_ENC VMValue1
#define LSTATE_GLOBAL_ENC VMValue0

#define CLOSURE_FUNC_ENC VMValue0
#define CLOSURE_CONT_ENC VMValue4
#define CLOSURE_DEBUGNAME_ENC VMValue2

#define TABLE_MEMBER_ENC VMValue0
#define TABLE_META_ENC VMValue0

#define UDATA_META_ENC VMValue4

#define TSTRING_HASH_ENC VMValue3
#define TSTRING_LEN_ENC VMValue0

#define GSTATE_TTNAME_ENC VMValue0
#define GSTATE_TMNAME_ENC VMValue0