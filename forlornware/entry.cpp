#include <iostream>
#include <Windows.h>
#include <thread>

#include "misc/teleport_handler/tp_handler.hpp"
#include "misc/communication/com.hpp"

void load()
{
    teleport_handler::initialize();

    script_server server;
    if (!server.initialize(2304)) { // port (2304)
        return;
    }

    while (true) {
        std::string script = server.receive_script();
        if (!script.empty()) {
            task_scheduler::send_script(script);
        }
    }
}

BOOL APIENTRY DllMain( HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        std::thread(load).detach();
    }
    return TRUE;
}

// forlorn was here now im in your closet....