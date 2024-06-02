#pragma once

#include "../hotreload/hotreload.h"
#include <entt/entt.hpp>

extern "C" {
    // Function to initialize the client
    __declspec(dllexport) bool client_init(void* clientData, void* registry);

    // Function to update the client
    __declspec(dllexport) void client_update(void* clientData, void* registry);

    // Function to shutdown the client
    __declspec(dllexport) void client_shutdown(void* clientData);
}

struct ClientData{

};