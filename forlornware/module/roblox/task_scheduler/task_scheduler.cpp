#include "task_scheduler.hpp"

void task_scheduler::set_proto_capabilities(Proto* proto, uintptr_t* c) {
    if (!proto) return;
    proto->userdata = c;
    for (int i = 0; i < proto->sizep; ++i)
        set_proto_capabilities(proto->p[i], c);
}

void task_scheduler::set_thread_capabilities(lua_State* l, int lvl, uintptr_t c) {
    *(uintptr_t*)((uintptr_t)(l->userdata) + 0x48) = c;
    *(int*)((uintptr_t)(l->userdata) + 0x30) = lvl;
}

uintptr_t task_scheduler::get_datamodel() {
    uintptr_t fake_datamodel = *(uintptr_t*)(update::offsets::datamodel::fake_datamodel);
    return fake_datamodel + update::offsets::datamodel::fake_datamodel_to_datamodel;
}

uintptr_t task_scheduler::get_script_context() {
    uintptr_t children_pointer = *(uintptr_t*)(get_datamodel() + update::offsets::instance::children);
    return *(uintptr_t*)(*(uintptr_t*)(children_pointer) + update::offsets::datamodel::script_context);
}

uintptr_t task_scheduler::get_lua_state() {
    auto addr = get_script_context() + 0x140 + 0x30 + 0x88; // globalstate, conversionoffset, decryptstate
    auto ptr = reinterpret_cast<const uint32_t*>(addr);
    return (uint64_t(addr - ptr[1]) << 32) | (addr - ptr[0]);
}