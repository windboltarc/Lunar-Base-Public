#include "task_scheduler.hpp"

void task_scheduler::set_proto_capabilities(Proto* proto, uintptr_t* c)
{
    if (!proto) return;

    proto->userdata = c;
    for (int i = 0; i < proto->sizep; ++i)
        set_proto_capabilities(proto->p[i], c);
}

void task_scheduler::set_thread_capabilities(lua_State* l, int lvl, uintptr_t c)
{
    *(uintptr_t*)((uintptr_t)(l->userdata) + 0x48) = c;
    *(int*)((uintptr_t)(l->userdata) + 0x30) = lvl;
}

uintptr_t task_scheduler::get_datamodel()
{
    uintptr_t fake_datamodel = *(uintptr_t*)(update::offsets::datamodel::fake_datamodel);
    return *(uintptr_t*)(fake_datamodel + update::offsets::datamodel::fake_datamodel_to_datamodel);
}

uintptr_t task_scheduler::get_script_context()
{
    uintptr_t children_pointer = *(uintptr_t*)(get_datamodel() + update::offsets::instance::children);
    return *(uintptr_t*)(*(uintptr_t*)(children_pointer) + update::offsets::datamodel::script_context);
}

uintptr_t task_scheduler::get_lua_state() {
    auto base = get_script_context() + 0x170; // globalstate offset + conversion offset (0x140 + 0x30)
    auto nigga = static_cast<uint32_t>(base + 0x88);
    auto ptr = reinterpret_cast<const uint32_t*>(base + 0x88);
    return (uint64_t(ptr[1] ^ nigga) << 32) | (ptr[0] ^ nigga);
}