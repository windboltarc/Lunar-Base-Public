#pragma once
#include <cstdint>
#include <lua.h>
#include <Windows.h>
#include <queue>

#include "Compiler/include/luacode.h"
#include "luau/BytecodeBuilder.h"
#include "luau/BytecodeUtils.h"
#include "luau/Compiler.h"

inline uintptr_t max_caps = 0xEFFFFFFFFFFFFFFF;

class bytecode_encoder : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* data, size_t count) override {
        for (auto i = 0u; i < count;) {
            uint8_t op = LUAU_INSN_OP(data[i]);
            const auto opLength = Luau::getOpLength(static_cast<LuauOpcode>(op));
            const auto lookupTable = reinterpret_cast<BYTE*>(update::lua::opcode_lookup);
            uint8_t newOp = op * 227;
            newOp = lookupTable[newOp];
            data[i] = (newOp) | (data[i] & ~0xff);
            i += opLength;
        }
    }
};

inline bytecode_encoder encoder;

inline std::string compile_script(const std::string& omegahacker) {
    static const char* mutable_globals[] = {
        "Game", "Workspace", "game", "plugin", "script", "shared", "workspace",
        "_G", "_ENV", nullptr
    };

    Luau::CompileOptions options;
    options.debugLevel = 1;
    options.optimizationLevel = 1;
    options.mutableGlobals = mutable_globals;
    options.vectorLib = "Vector3";
    options.vectorCtor = "new";
    options.vectorType = "Vector3";

    return Luau::compile(omegahacker, options, {}, &encoder);
}

namespace globals {
    inline lua_State* roblox_state;
    inline lua_State* forlorn_state;
    inline bool loaded = false;

    inline std::queue<std::string> teleport_queue = {};
    inline std::queue<std::string> execution_queue = {};
}