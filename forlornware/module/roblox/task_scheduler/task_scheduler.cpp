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
    if (!l || !l->userdata) return;

    auto extra_space = (uintptr_t)(l->userdata);
    *(uintptr_t*)(extra_space + 0x48) = c;
    *(int*)(extra_space + 0x30) = lvl;
}

uintptr_t task_scheduler::get_datamodel()
{
    uintptr_t fake_datamodel = *(uintptr_t*)(update::offsets::datamodel::fake_datamodel);
    return *(uintptr_t*)(fake_datamodel + update::offsets::datamodel::fake_datamodel_to_datamodel);
}

uintptr_t task_scheduler::get_script_context()
{
    uintptr_t datamodel = get_datamodel();

    uintptr_t children_pointer = *(uintptr_t*)(datamodel + update::offsets::instance::children);
    uintptr_t children = *(uintptr_t*)(children_pointer);
    return *(uintptr_t*)(children + update::offsets::datamodel::script_context);
}

uintptr_t task_scheduler::get_lua_state()
{
    uintptr_t script_context = get_script_context();
    size_t offset = 1 * 0x158 + 0x30;
    uintptr_t encrypted_state = ((script_context + 0x140) + offset) + 0x88;
    return (static_cast<uint64_t>(*(uint32_t*)(encrypted_state + 0x4) - (uint32_t)encrypted_state) << 32)
        | *(uint32_t*)(encrypted_state)-(uint32_t)encrypted_state;
}