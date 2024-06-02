#pragma once

#include "../hotreload/hotreload.h"
#include <entt/entt.hpp>

extern "C" {
    // Function to initialize the client
    __declspec(dllexport) bool client_init(ClientData* clientData, void* registry);

    // Function to update the client
    __declspec(dllexport) void client_update(ClientData* clientData, void* registry);

    // Function to shutdown the client
    __declspec(dllexport) void client_shutdown(ClientData* clientData);
}

struct ClientData{
    class GLFWwindow* window;
    unsigned int vertex_buffer, vertex_shader, fragment_shader, program;
    int mvp_location, vpos_location, vcol_location;
};